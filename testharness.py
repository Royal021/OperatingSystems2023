#!/usr/bin/env python3

import subprocess
import sys
import asyncio
import asyncio.subprocess
import base64,bz2
import os
import time
import random
import io
import re
import zlib

TIMEOUT=25



async def testIt():

    P = await runIt()
    try:
        await waitForLine(P,f"Hello!")
        time.sleep(0.5)
        regs = await getRegisters(P)
    finally:
        await quitQemu(P)

    if regs[13] < 0x700000 or regs[13] > 0x800000:
        print("\n\nsp is bad:",hex(regs[13]))
        return
    if regs[15] < 0x400000 or regs[15] > 0x500000 :
        print("\n\npc is bad:",hex(regs[15]))
        return

    print("\n\n\nOK!")

    return


async def waitForLine(P,txt):
    for i in range(2000):
        x = await readline(P)
        if txt in x:
            return
    raise Exception(f"Never found expected text: {txt}")


async def getRegisters(P):
    P.stdin.write(b"\ninfo registers\n~c\ninfo registers\n\n")
    await P.stdin.drain()
    data=[]
    inMonitor=False
    while True:
        line = (await readline(P)).strip()
        if not inMonitor:
            if line.startswith("(qemu)"):
                inMonitor=True
            else:
                continue
        if not line.startswith("(qemu)") and " monitor " not in line and line != "":
            data.append(line)
        if line.startswith("(qemu)") and len(data) > 0:
            break
    data = "\n".join(data)
    regs = re.findall(r"R\d\d=([A-Fa-f0-9]{8})",data)

    print("data=",data)
    print("REGS=",regs)

    return [int(q,16) for q in regs]

async def runIt():
    python = sys.executable

    P = await asyncio.create_subprocess_exec(
        python, "-u", "make.py",
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.STDOUT
    )
    return P

async def main():
    await testIt()
    return

async def quitQemu(P):
    try:
        P.stdin.write(b"\nquit\n\n~c\nquit\n")
        await P.stdin.drain()
        while await readline(P):
            pass
    except:
        pass
    try:
        P.terminate()
        await P.wait()
    except:
        pass

async def readline(P):
    line=""
    while True:
        task = asyncio.create_task( P.stdout.read(1) )
        done,pending = await asyncio.wait( [task] , timeout=TIMEOUT )
        if len(done) == 0:
            raise Exception("Timeout: No output produced")
        c = done.pop().result()
        if len(c) == 0:
            return line
        line += c.decode(errors="backslashreplace")
        if c == b'\n':
            print (line)
            return line
        if len(line) > 16384:
            raise Exception("Too much data for one line")


asyncio.run(main())
