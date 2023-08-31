#!/usr/bin/env python3

import subprocess
import sys
import asyncio
import asyncio.subprocess
import base64,bz2
import os
import zlib

async def testIt():

    with open("testsuite.c","r") as fp:
        ts = fp.read()

    if zlib.crc32(ts.strip().encode()) != 0xacd2ff9d:
        print("testsuite.c doesn't match what we expect")
        return

    P = await runIt()
    try:
        actual = await getImage(P)
        if actual == None:
            return
        if not await compareImage(actual):
            print("Image mismatch")
            return
    finally:
        await quitQemu(P)

    try:
        ts2 = ts.replace("years","yarrs")
        with open("testsuite.c","w") as fp:
            fp.write(ts2)

        P = await runIt()
        try:
            actual = await getImage(P)
            if actual == None:
                print("Failed to get second image")
                return
            if False == await compareImage(actual):
                print("Good images are good and bad images are bad. Good.")
            else:
                print("Even bad images test as good?!?! I call shenanigans!")
        finally:
            await quitQemu(P)
    finally:
        with open("testsuite.c","w") as fp:
            fp.write(ts)

async def getImage(P):
    for i in range(1000):
        s = await readline(P)
        print(s)
        if not s:
            print("Process exited without printing required string")
            return None
        if s == "DONE\n":
            try:
                os.unlink("output.ppm")
            except FileNotFoundError:
                pass
            P.stdin.write(b"~c\nscreendump output.ppm\ninfo status\n")
            await P.stdin.drain()
            for i in range(100):
                s = await readline(P)
                if "VM status" in s:
                    break
            else:
                print("Something went wrong")
                return None

            with open("output.ppm","rb") as fp:
                actual = fp.read()
            return actual

    print("Did not find word 'DONE'")
    return None


async def compareImage(actual):
    exp = bz2.decompress(base64.b64decode(expected))
    if exp == actual:
        return True
    else:
        return False

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
    await asyncio.sleep(0.5)
    try:
        P.terminate()
        await P.wait()
    except:
        pass

async def readline(P):
    line=b""
    while True:
        c = await P.stdout.read(1)
        if len(c) == 0:
            return line.decode(errors="ignore")
        line += c
        if c == b'\n':
            line = line.decode(errors="ignore")
            print (line)
            return line
        if len(line) > 16384:
            raise Exception("Too much data for one line")

expected=(
"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"
)

asyncio.run(main())
