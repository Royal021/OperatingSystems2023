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

if zlib.crc32(ts.strip().encode()) != 0x9812256b:
    print("testsuite.c doesn't match what we expect")
    sys.exit(1)

async def testIt():

    P = await runIt()
    try:
        await waitForLine(P,b"START")
        data=b""
        while True:
            tmp = await readline(P)
            if tmp == b"DONE\n":
                break
            data += tmp

    finally:
        await quitQemu(P)

    data = data.decode(errors="backslashreplace")
    expected=["ABOUT   TXT","ARTICLE1TXT", "ARTICLE2TXT",
        "ARTICLE3TXT", "ARTICLE4TXT", "ARTICLE5TXT",
        "ARTICLE6TXT", "BILLOF~1TXT"
    ]
    for e in expected:
        if e not in data:
            print("\n\n\nIncorrect output")
            return

    print("\n\n\nOK!")

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
            return line
        line += c
        if c == b'\n':
            print (line)
            return line
        if len(line) > 16384:
            raise Exception("Too much data for one line")


asyncio.run(main())
