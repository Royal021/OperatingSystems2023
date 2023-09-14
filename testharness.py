#!/usr/bin/env python3

import subprocess
import sys
import asyncio
import asyncio.subprocess
import base64,bz2
import os
import time
import zlib

TIMEOUT=3

with open("testsuite.c","r") as fp:
    ts = fp.read()

""" if zlib.crc32(ts.strip().encode()) != 0x2ffd53f5:
    print("testsuite.c doesn't match what we expect")
    sys.exit(1) """

async def testIt():

    ok=False
    try:
        if not await doTest(1,"abt32","PREFETCH ABORT"):
            return
        if not await doTest(2,"und32","UNDEFINED OPCODE"):
            return
        if not await doTest(3,"svc32","SVC INT"):
            return
        ok=True
    finally:
        with open("testsuite.c","w") as fp:
            fp.write(ts)
        if ok:
            print("All OK")
        else:
            print("Bad.")

async def doTest(intNum,expectedMode,description):

    print("Testing",description,"...")

    with open("testsuite.c","w") as fp:
        fp.write(f"#define INTERRUPT_TEST {intNum}\n")
        fp.write(ts)

    P = await runIt()
    try:
        await waitForLine(P,description)
        time.sleep(0.5)
        mode,pc = await getModeAndPC(P)
        if mode != expectedMode:
            print("Didn't get expected mode",expectedMode)
            return False
        if pc < 0x8000 or pc > 0x100000:
            print("pc is bad")
            return False
        print(description,": OK")
    finally:
        await quitQemu(P)
    return True

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
    P.stdin.write(b"\nquit\n\n~c\nquit\n")
    await P.stdin.drain()
    while await readline(P):
        pass
    try:
        P.terminate()
        await P.wait()
    except:
        pass

async def readline(P):
    line=b""
    while True:
        task = asyncio.create_task( P.stdout.read(1) )
        done,pending = await asyncio.wait( [task] , timeout=TIMEOUT )
        if len(done) == 0:
            raise Exception("Timeout: No output produced")
        c = done.pop().result()
        if len(c) == 0:
            return line.decode(errors="ignore")
        line += c
        if c == b'\n':
            line = line.decode(errors="ignore")
            print (line)
            return line
        if len(line) > 16384:
            raise Exception("Too much data for one line")

asyncio.run(main())
