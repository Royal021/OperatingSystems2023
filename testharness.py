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

TIMEOUT=3
make = os.getenv("MAKE3701","make.py")

with open("user/hello-pfa.c","r") as fp:
    ts = fp.read()

async def testIt():

    try:
        with open("user/hello-pfa.c") as fp:
            tmp=fp.read()
        idx1=tmp.find("//!!");
        idx2=tmp.find("//!!",idx1+1)
        assert idx1 != -1
        assert idx2 != -1
        tmp = tmp[:idx1]+tmp[idx2:]
        with open("user/hello-pfa.c","w") as fp:
            fp.write(tmp)
        P = await runIt()
        try:
            await waitForLine(P,f"Hello!")
            time.sleep(0.5)
            regs,mode = await getRegisters(P)
        finally:
            await quitQemu(P)
        if regs[15] < 0x400000 or regs[15] > 0x800000:
            print("Bad pc for no exception: Got",hex(regs[15]))
            return
        if mode != "usr32":
            print("Bad mode for no exception:",mode)
            return
            
        with open("user/hello-pfa.c","w") as fp:
            fp.write(ts)
        P = await runIt()
        try:
            await waitForLine(P,f"Hello!")
            time.sleep(0.5)
            regs,mode = await getRegisters(P)
        finally:
            await quitQemu(P)
        if regs[15] >= 0x400000: 
            print("Bad pc for PFA: Got",hex(regs[15]))
            return
        if mode != "abt32":
            print("Bad mode:",mode)
            return
    finally:
        with open("user/hello-pfa.c","w") as fp:
            fp.write(ts)

    print("\n\n\nOK!")
    return


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
    
    i = data.find("PSR=")
    assert i != -1,data
    j = data.find("\n",i)
    assert j != -1
    mode = data[i:j].strip().split()[-1]
    

    return [int(q,16) for q in regs],mode


async def waitForLine(P,txt):
    for i in range(2000):
        x = await readline(P)
        if txt in x:
            return
    raise Exception(f"Never found expected text: {txt}")


async def getModeAndPC(P):
    P.stdin.write(b"\ninfo registers\n~c\ninfo registers\n")
    await P.stdin.drain()
    pc=-1
    mode="?"
    while True:
        line = (await readline(P)).strip()
        if line.startswith("R12="):
            idx = line.find("R15=")
            assert idx >= 0
            pc = int(line[idx+4:],16)
        elif line.startswith("PSR="):
            mode = line.split()[-1]
            return mode,pc

async def runIt():
    python = sys.executable

    P = await asyncio.create_subprocess_exec(
        python, "-u", make,
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
