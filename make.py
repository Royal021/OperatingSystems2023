import configparser
import subprocess
import os
import sys
import lab11

inifile=configparser.ConfigParser()
inifile.read("config.ini")
conf=inifile["config"]

cc=conf["compiler"]
link=conf["linker"]
qemu=conf["qemu"]
python=conf["python"]

cflags=[
    "-std=c11",                     #Use C-11 standard
    "-target", "arm-arm-none-eabi", #CPU family
    "-mcpu=arm1176jzf-s",           #CPU model
    "-fpic",                        #position independent code
    "-ffreestanding",               #Don't use standard functions
    "-nostdlib",                    #Don't use C library
    "-Wall",                        #All warnings
    "-Werror",                      #Warnings are errors
    "-Wshadow",                     #Warn about shadowed parameters
    "-Wdouble-promotion",           #Warn about float->double
    "-Wundef",                      #Warn about undefined macros
    "-fno-common",                  #no common (bss) block
    "-fstack-usage",                #record stack usage
    "-Wframe-larger-than=1024",     #warn about large stack frames
    "-Wconversion",                 #warn about type conversions
    "-Wno-format-invalid-specifier",#ignore printf format string
    "-Wno-format-extra-args"        #ignore extra printf args
]

linkflags=[
    "-T", "linkerscript.txt",       #linker script
    "-Map", "kernel.map",           #make a map file (debugging)
    "-o", "kernel.elf"              #output file
]

def doIt(cmd):
    try:
        print(" ".join(cmd))
        subprocess.check_call(cmd)
    except subprocess.CalledProcessError:
        sys.exit(1)

objectfiles=[]
for filename in os.listdir("."):
    if filename.endswith(".c"):
        obj=filename+".o"
        doIt( [cc] + cflags + ["-c", "-o", obj, filename] )
        objectfiles.append(obj)

doIt( [link] + linkflags + objectfiles )

doIt( [python,"fool.zip",
    "sd.img", "create","64",
    "cp","article7.txt","ARTICLE7.TXT",
    "cp","article1.txt","ARTICLE1.TXT",
    "cp","article2.txt","ARTICLE2.TXT",
    "cp","article3.txt","ARTICLE3.TXT",
    "cp","article4.txt","ARTICLE4.TXT",
    "cp","article5.txt","ARTICLE5.TXT",
    "cp","article6.txt","ARTICLE6.TXT",
    "cp","billofrights.txt","BILL.TXT",
    "cp","const.txt","CONST.C",
])

doIt( [ qemu,
    "-m", "512",                #memory (512MB)
    "-M", "raspi0",             #machine (raspberry pi 0)
    "-kernel", "kernel.elf",    #kernel file
    "-echr", "126",             #escape character (~)
    "-serial", "mon:stdio",      #connect serial to console
    "-drive", "file=sd.img,if=sd,format=raw"
])