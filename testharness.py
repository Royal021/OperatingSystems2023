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

TIMEOUT=3

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
                    day = int(M.\Nnvr 0&R
     @      @      @hlvz$ zhnt(P\rlvz h(
$   @      @      @ ljb|jh <"`r\hL.d~d^j(4*"@      @      @    `fJnnd( :@nt(@:\Nnvr 0,R
     @      @      @T 8*`hRd.ljvhRd*( 4 @      @      @      @xfbr$      @      @      @  hj~\h,$   @      @      @     @Hx.$  @      @      @      @nvr$@      @      @      @ ljb|jh,$   @      @      @     @fbnnl(      @      @      @  0 @      @      @      @ 0(4 @      @      @      @,2
$  @      @      @  ("@      @      @    `hRdFhbl@ `bt&@xp`jfhJD`rh* ZT (4 @      @      @   `rPhnbJhRL <$0(t      @      @      @   p`dRt( DdBHthjj*@^ lhb|
@ (&`xR\(
$   @      @      @     @NtThbzJf<"@lbXf
     @      @   `jXf:$   @      @      @ ppdr\h"Ll.`Fd`vhb~@hnb*`^\lhjl* D*"hhr\J
     @      @      @fnvXhRZr"8*@LBrf
$  @    `JXd:$  @      @ p`rf&      @     @ ppdr\h"DzphdBdnvz&@Xnd:$$@hnd*@      @    &@|rdrxjd\
    @Lr hhr\Jhn$`xR\r:$  @    `RLlhjl*\Lnd(`hJXtdbHl\BdZ62T:R >" :"t      @     @`hnt(0Dldrhj@hn`* DddjhjhJFldbz
X("`r``Jrp&`r@tvppj(\(
$   @      @rdrxjd\
$   @nndN|BZrNN6Zhdd
$   @nr$`jp`hn$`jp`bv`j(4      @hf$`jp`llnln^V<>"@lBXd:$  @      @ lhn|NlfrF^@ DdbxfJ
     @      @rhj|(Dn"`d~\j: Hjrffnd&`x^\ ldbz
@*fzp \XndnlbZJ
     @      @rdbb@  `jxfJ
     @  ppdR\( Lh~\Nn`jjj&@dfj*`h^bd"@^D
$   @hf$phPJllnl.@\lfr&`Bd LN&8@n dhn<Nhbbrd*@Bnvr(`hP pn`~dhnndz&   `r@nv `x^\N`jjjf:$   @   `l^ddzp `R\dzp`jFhd8$  @      @ hb,`Jp.pn`~dhj"8::@lpf:@      @      @pplb|(PLjrvfR\ pn`~dhn`jj* D*"`zp \hlvth\Bd*
$  @      @     @dttrd   p`dRt((4 @ ppdr\h(
$   @rhj|(R
$   @f `zp`JtdbHhBh <:*`\^:$   @   p`dRt( D|^h: PzfJHbvrtdJ\ d`rh
^hnb*`L^ `n`jFV"(
$  @dnpf*4      @pplb|(PNlvh*4@rjjl.@Lrdfj(@Htd*~hRZ"(
4 @ ppdr\h(
$   @rhj|(R    ``dnt(    pfF^d"8* "`
    @`hnt(0 @  DdbfR l`b$ @      @      @     b0$"$   `rPnndN|BZrNN&24      @pplb|(PLlnl.@\lf"`d^\r:$   @      @  (& .j"(
$  @    `fFrd" 6:@4
$   @lpf:@      @rhj|(Dn"hh~\Nn`jj*@Dntr64 @      @ (&   J(
$  @hf,`n^hhnbz&@
     @  ppdR\( DvdJBhnn,`hRd"`d~\j:     @     @4&"$@      @bnvd* V 04
4 @ djxf
t      @ ppdr\h"Ll.`Fd`vhb~@hnb*`D^tr>$  @    6 @$",@  ppdR\( Th~hB pff~dJ      @      @     Xtp,pfF^d**&$
D
`r~b|@df$pnBRFlvDxR\(P(xhph:$   @nr$`2@R pdb|NJ2000 t      @ x 0:@B`jr(`dJdlhb|
P(
$   @   `r@xt `r@:$   @      @rdrxjd\    `dBrf"@jpFpthb~P"LdzlJdflvj|@xp`jfhJ t`zp(tzvxph:D
dbfrb"`hj@dvHj~HJndP@&P :$   @.pvhhR\vrlrh
P"\hlr\L pdjnRfdrtVx\|\llb|L^rdbnrfhrp^h<R    `Bnhv P \fdhj,|Hdhn, @  p`&:Z
    @Zdd:*$>D    `nPld"Phdj:$   @   `xR\ <" pBnhv pdJBlhjl*P (.tvhdR((
$  @    `RLlhjl*\f`rtxfnRh( T$"d"(:4 @      @  hbh0@ lhb|
\hnd(0D$4>"$@      @   `bffrt `rHp><"0 @      @   p`@ hj|(PXndZfrHp48\*8"l
     @  `jxRLlhjl*\f`rtxfnRh( T@f$"(:4 @      @  hj~HJ<"hhr\Jrrhhr(PZ.2R@      @   pdJhrl$`z^H,p`TBxnd&`HJ ptj\Rh(:$  @pxrhp^\<"pvr&\xdbvjhBld
4 @ P 0:@B`jr(`Bfn`nb>\FdbrhJ~fbppd~FJr^fzpJF
     @  pprhPn,  $Zj,  dzBV.px"$      @rv`hrzrzjlfR^rvbt`d^drv&\`D.$  @    `fhnvr8zBfn`nb>\fbppd~FJr.T@R`

     @  pvhHJr<brfr\hn.|fjDrlffjffRV@H^j(        pdJhrl$P rzjl&@Hf hjbR\(:$  @`vfbr(@drvHr(P
    @dttrdrzjl&@Hf prjRhdnr*P R
    @hx:$  @    @ \tdhb<\nhv`*pDnprjrh8\l|lFx\tjrXxD
     @  `rnBR P(|fhHn,dxdBR((
$  @    `nPld"`bnBt pdjBHhnd*P R
     @      @`rv4 @ dz`fJ`:$   @   p`Bf
    @hx:$  @    @ \drljr\Bd*(4 @     @B`jr(@ \`jr(0   `jpFpt84 @     @`rrtBfn`&`hJLrdbbhXRd*P 24   `xR\<"$4 @ tn`rXJTptj*4      @t`rf6 z`r~b|FR.`vdjBh^v`rfPP,tvhH^t,tdjBH0*" @      @nnd*x`Jdhjl. z`vfbr(@rzjlfR^vbjr(@t`rfV:@ thbzJ^t<RHRZ
TV  @      @f hhjPnnd*2 z 084 @      @  pdbRf Dz`fJ`hnn,0D(lfj~j(tNl&`~jhtv p`d^tbfj(R      @ `&0:@Hnd*|`^`(.tdjfjt((4 @     @R l`j<PF <:*  t      @     @dttrd<@Xnd
$  @    `XRd"(6:@Fddbf~HJdrtd~df"`dbfVf`rnpdJ``bf*$      @hf$`& z `$^xN
     @      @rhj|( Phnd*@      @   pdJhrl$`xR\
     @  hb,@Xn(hhr\J <$0",f48$  @      @ pdbrfJDz`fj`hnn, Dh^ lrjf@`v`"`L^ lnl*@Xnd"$`r~b|FR.ptj<PZhn, 2