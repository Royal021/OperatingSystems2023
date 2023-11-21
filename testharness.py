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

with open("user/a.c","r") as fp:
    adata = fp.read()

async def testIt():

    r = random.randrange( ord('A'), ord('Z') )
    r = chr(r)

    ok=[False]*3
    spurious=False

    try:
        with open("user/a.c","w") as fp:
            fp.write( adata.replace('"A"',f'"{r}"') )
        P = await runIt()
        try:
            await waitForLine(P,"qemu-system-arm")
            while True:
                data = await readData(P,100)
                if r*5 in data:
                    ok[0]=True
                if "B"*5 in data:
                    ok[1]=True
                if "C"*5 in data:
                    ok[2]=True
                for x in "ADEFGHIJKLMNOPQRSTUVWXYZ":
                    if x != r and x*5 in data:
                        spurious=True
                        break
                        return
                if False not in ok:
                    break

        finally:
            await quitQemu(P)

    finally:
        with open("user/a.c","w") as fp:
            fp.write(adata)

    if spurious:
        print("\n\n\nSpurious letters?")
        return

    if False in ok:
        print("\n\n\nBad")
        return

    print("\n\n\nAll OK")
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

def decompressText(data):
    return bz2.decompress(base64.b64decode(data)).decode()


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


async def readData(P,amount):
    line=""
    while True:
        task = asyncio.create_task( P.stdout.read(amount) )
        done,pending = await asyncio.wait( [task] , timeout=TIMEOUT )
        if len(done) == 0:
            raise Exception("Timeout: No output produced")
        c = done.pop().result()
        if len(c) == 0:
            return line
        line += c.decode(errors="backslashreplace")
        if len(line) >= amount:
            print(line)
            return line
        amount -= len(c)
        if len(line) > 65536:
            raise Exception("Too much data for one read")

asyncio.run(main())
