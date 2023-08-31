#!/usr/bin/env python3

import subprocess
import sys
import asyncio
import asyncio.subprocess
import base64,bz2
import os

async def testIt(P):
    for i in range(1000):
        s = await readline(P)
        print(s)
        if not s:
            print("Process exited without printing required string")
            return
        if s == "DONE\n":
            await doImageComparison(P)
            return
    print("Never found expected output.")

async def doImageComparison(P):
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
        return

    with open("output.ppm","rb") as fp:
        actual = fp.read()
    exp = bz2.decompress(base64.b64decode(expected))
    if exp == actual:
        print("Images match! Good!")
    else:
        #with open("expected.ppm","wb") as fp:
        #    fp.write(exp)
        print("Output does not match expected results")

async def main():
    python = sys.executable

    P = await asyncio.create_subprocess_exec(
        python, "-u", "make.py",
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.STDOUT
    )
    try:
        await testIt(P)
    finally:
        P.stdin.write(b"\nquit\n\n~c\nquit\n")
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
            line = line.decode(errors="ignore")
            print (line)
            return line
        if len(line) > 16384:
            raise Exception("Too much data for one line")

expected=(
"QlpoOTFBWSZTWe8Gmp0AMLz/qdJZJIJJJNNJJAJABJBJIBIAQEAAAAQACMACfAAkgankTQKb"
"VHqaepoaA0D0hhoZMgZGIMTJoaYMNDJkDIxBiZNDTAiREaTUTQZPSDQGhkgepuUa49YXvE5u"
"7ZNtisSy9+DnjGk3CjJkoqSYMSDCPE4hckYwEqYmOEL41kF2gGouYyIsl42CUy0DGbLCtFEx"
"taNxdRhvSZhUqiSSVRNyFkcQoxiirLugHbehdyOuHV4m+FpmxFROL1mi72yYJnFBUUtiooi8"
"wRQRrNJyACipRYMhI74oqAWMkB3jxsKKCqdAUQioQUICwGIAeHf8cI4UBgDFSCBAgDFYpES4"
"qEL69mFduteXwzaJ/YKiqJ0EUF7Dz7uuAEF8/Ts8tGjqeMLlQ8CrnJRR3R1BMuJIGPl7H87V"
"f0qpcXW2C9dHxknNamSodEiZEJZcP1qwnWyUJsYSwKwhwhV4SB9eJNtSxYHoKU1MlmzCAmvk"
"mcD5ILL6spfbKQ7aITIUOCnwo1dYztfaVgNdDz3CzCfxnuntPXub7eaQo7Smywl2/59vrZsZ"
"hCdffzO/Jlv0rGNr3NvENOcgZD0G/Su8uPPn125PTQ6JVdB7N3OI1L6U8tWUyRna3JkatPvK"
"IY7q8bMKkUobliG9zR4zjFbXVDSEE4dgVdnDEyRVf8XckU4UJDvBpqdA"
)

asyncio.run(main())
