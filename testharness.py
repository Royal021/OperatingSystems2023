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

with open("user/hello.c","r") as fp:
    ts = fp.read()

async def testIt():

    try:
        with open("user/hello.c","w") as fp:
            fp.write( decompressText(helloNoFault) )
        P = await runIt()
        try:
            await waitForLine(P,"Hello!")
            time.sleep(0.5)
            regs,mode = await getRegisters(P)
        finally:
            await quitQemu(P)

        if mode != "svc32" and mode != "usr32":
            print("Bad CPU mode for hello-nofault.c")
            return

        with open("user/hello.c","w") as fp:
            fp.write( decompressText(helloPfa) )
        P = await runIt()
        try:
            await waitForLine(P,"Hello!")
            time.sleep(0.5)
            regs,mode = await getRegisters(P)
        finally:
            await quitQemu(P)

        if mode != "abt32":
            print("Bad CPU mode for hello-pfa.c")
            return

        with open("user/hello.c","w") as fp:
            fp.write( decompressText(helloData) )
        P = await runIt()
        try:
            await waitForLine(P,"Hello!")
            time.sleep(0.5)
            regs,mode = await getRegisters(P)
        finally:
            await quitQemu(P)

        if mode != "abt32":
            print("Bad CPU mode for hello-da.c")
            return


    finally:
        with open("user/hello.c","w") as fp:
            fp.write(ts)

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

helloNoFault=(
"QlpoOTFBWSZTWSE4PUQAAAjfgEQQeHXgCABAAA4/59+KIACVRCTIZABiGgB6RoGSFPNSfqRk"
"NGjJo9Royeo8mkacFgjnwVEaEiCHk4H1pZa5hxGczLkxY+aq452JxfulRqgaCBpSsHTwv1es"
"VAQl2J2DAXj4TMDAYCOfW+im3XQ4TYXdjOpQxulqoAj4sVyD8OjXiVEV4QcWumt4NF+LuSKc"
"KEgQnB6iAA=="
)
helloPfa=(
"QlpoOTFBWSZTWY2lz4wAAA3fgEQQeHX0mhFCRA4/59+qMADaLDKTJqehPRPSDRpphA000Ymg"
"YAaaNBpiAAABkAMlMmnqaIGmgyNANABo2owkNhTIkEW6JzohG5yIJ2qQAGEi02x1eemq7FTm"
"eErcJhheYt7i4sECYUBeq1Ql2UxDlkmlV7Y0SigoUV2CEOTBMHhYEs56w1BWzs1kLlrjOc9y"
"vGVBSMT7xxNRfQXxmaqjmzN4pyts6TwJeQ1UiMThZw+yG4boMjX50hs1ZomlGAukvWooVQBd"
"wgBnyzxIKxKtj7DNWj2BtDTeZ6ojQfJsAzUX/F3JFOFCQjaXPjA="
)
helloData=(
"QlpoOTFBWSZTWdr4nesAAAtfgEQQeHX1CgBAAA4/59/KMAC7axGpAABoAaAAyANFTyNqbVPJ"
"DTyhoaZHqAANIaRMARgARptTCZoogDa56DfC1mDWtCIDdIgSiAxKliGj/N5SmMTjZhWSgUCo"
"+Oip2MOFy7/RNw3UjadwaGkgi/ODwC8hVXpDDMsFH7VoqsE0SQnoLnksHoUO02IMRiSOkIHz"
"gP52I6KJXlSxApSd8QAVztQsxj6JrsDvxowNYU73xpqyM+xdyRThQkNr4nes"
)

asyncio.run(main())
