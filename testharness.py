#!/usr/bin/env python3

import subprocess
import sys
import asyncio
import asyncio.subprocess

async def testIt(P):
    for i in range(1000):
        s = await readline(P)
        print(s)
        if not s:
            print("Process exited without printing required string")
            return
        if s == "We the People of the United States\n":
            print("Found expected line. Good.")
            return
    print("Never found expected output.")


async def main():
    python = sys.executable

    P = await asyncio.create_subprocess_exec(
        python, "-u", "make.py",
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.STDOUT
    )
    await testIt(P)
    P.stdin.write(b"~cquit\n")
    await P.stdin.drain()
    await asyncio.sleep(0.5)
    try:
        P.terminate()
    except:
        pass

    return


async def readline(P):
    line=b""
    while True:
        c = await P.stdout.read(1)
        if len(c) == 0:
            return line.decode(errors="ignore")
        line += c
        if c == b'\n':
            return line.decode(errors="ignore")
        if len(line) > 16384:
            raise Exception("Too much data for one line")


asyncio.run(main())
