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

TIMEOUT=10

class Expectation:
    def __init__(self,a,b):
        self.shortname=a
        self.longname=b
        self.shortok=False
        self.longok=False


async def testIt():

    os.environ["FOOL_NOW"] = "2023-Sep-08 01:02:04"
    expectedDate = os.getenv("FOOL_NOW",None)

    points=0
    summary=io.StringIO()
    names=[]


    with open("lab11.py") as fp:
        lab11=fp.read()
    try:

        with open("lab11.py","w") as fp:
            print("import subprocess",file=fp)
            print("def copy(python,imgfile):",file=fp)
            print('    subprocess.check_call([python, "fool.zip", imgfile,',file=fp)


            toDelete = random.randrange(1,7)
            deletedFname = f"ARTICLE{toDelete}.TXT"

            for i,n in enumerate([7,1,2,3,4,5,6]):
                fname = "article{}.txt".format(n)
                mixedcasefname = fname[:i] + fname[i].upper()+fname[i+1:]
                print('"cp","{}","{}",'.format(fname,mixedcasefname), file=fp)
                if i != toDelete:
                    names.append( (fname.upper(),mixedcasefname) )
            print('"cp","const.txt","cOnst.txt",', file=fp)
            names.append( ("CONST.TXT","cOnst.txt") )
            print('"cp","article2.txt","my fancy filename.txt",',file=fp)
            names.append( ("MYFANC~1.TXT","my fancy filename.txt" ) )
            print('"cp","article3.txt","this is a really long filename.c",',file=fp)
            names.append( ("THISIS~1.C","this is a really long filename.c") )
            print('"cp","billofrights.txt","BILL.TXT",',file=fp)
            names.append( ("BILL.TXT","BILL.TXT") )
            print('"cp","billofrights.txt","BILL.XA",',file=fp)
            names.append( ("BILL.XA","BILL.XA") )

            now=time.time()
            t=now-int(now)
            t *= 1000
            t = int(t)
            t = "%03d" % t
            n = "A"+t+".TXT"
            print('"cp","billofrights.txt","{}",'.format(n),file=fp )
            names.append( (n,n) )


            name2L = "a" + t + "c def ghij klmn opq.txt"
            name2S = "A" + t + "CD~1.TXT"
            print('"cp","billofrights.txt","{}",'.format(name2L),file=fp )
            names.append( ( name2S, name2L ) )

            print(f'"rm","{deletedFname}"',file=fp )

            print("])",file=fp)

        P = await runIt()
        try:
            await waitForLine(P,"START")
            lines=[]
            while True:
                tmp = await readline(P)
                if tmp == "DONE\n":
                    break
                lines.append(tmp)
        finally:
            await quitQemu(P)
    finally:
        with open("lab11.py","w") as fp:
            fp.write(lab11)





    print("Note: Deleted file",deletedFname)

    expected=[]
    for s,l in names:
        expected.append( Expectation( s,l ) )


    gotTimes = True;
    timerex = re.compile(r"(\d{4})-(\d{2})-(\d{2})\s+(\d{1,2}):(\d{2}):(\d{2})");

    if expectedDate == None:
        assert 0,"expectedDate is none!"
    else:
        expectedDate = time.mktime(time.strptime(expectedDate,"%Y-%b-%d %H:%M:%S"))

    for line in lines:
        ok=False

        for exp in expected:
            shortname = exp.shortname
            longname = exp.longname
            i = line.find(shortname)
            if i != -1:
                if exp.shortok:
                    print("Duplicate short name: " + shortname)
                    return
                j = i+len(shortname)
                if j < len(line) and line[j] not in " \t\n\r":
                    pass
                else:
                    exp.shortok = True
                    ok = True

            i = line.find(longname)
            if i != -1:
                if exp.longok:
                    print("Duplicate long name: " + longname)
                    return
                j = i+len(longname)
                if j < len(line) and line[j] not in " \t\n\r":
                    pass
                else:
                    exp.longok = True
                    ok = True

        if ok:
            # ~ print("OK:",line)
            if gotTimes:
                M = timerex.search(line)
                if M:
                    year = int(M.group(1))
                    month = int(M.group(2))
                    day = int(M.group(3))
                    hour = int(M.group(4))
                    minute = int(M.group(5))
                    second = int(M.group(6))
                    T = time.mktime((
                        year,
                        month,
                        day,
                        hour,
                        minute,
                        second,
                        0,
                        0,
                        -1
                    ))
                    timeDiff = abs( expectedDate - T )
                    if(timeDiff > 4):
                        print("Bad time on line " + line)
                        gotTimes = False
                else:
                    print("No creation time on line " + line)
                    gotTimes = False
        else:
            pass
            # ~ print("Extraneous line:", line)
            # ~ return

    for line in lines:
        if line.find(deletedFname[1:]) != -1:
            print("Deleted file (",deletedFname,") appears in output.")
            return


    longNamesOK=True
    for exp in expected:
        if exp.longok == False:
            longNamesOK = False;
            print("No bonus: Missing long name "+exp.longname)
            break
    else:
        print("Long names seem to be OK")

    #if the long names are OK, we don't care about the short ones.
    if not longNamesOK:
        for exp in expected:
            if exp.shortok == False:
                print("Missing short name " + exp.shortname)
                return

    print()
    print()
    print()


    if expectedDate == now:
        print("Note: Used current date/time for checks")
    else:
        print("Note: Using forged date/time")

    print()
    print()
    print()

    score = 100
    print(    "Basic lab                     +100%")

    if(longNamesOK):
        print("Long name bonus:              + 75%")
        score += 75
    else:
        print("No long name bonus:           +  0%")

    if(gotTimes) :
        print("Creation time bonus:          + 25%")
        score += 25
    else:
        print("No creation time bonus:       +  0%")

    print("Total score:                  ",str(score)+"%")






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


asyncio.run(main())
