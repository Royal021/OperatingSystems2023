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
userlinker=conf["userlinker"]

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



usercflags = cflags[:]

userlinkflags=[
    "-T", "userlinkerscript.txt"
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

for filename in os.listdir("user"):
    if filename.endswith(".c"):
        filename = os.path.join("user",filename)
        obj=filename+".o"
        exe=filename.replace(".c",".exe")
        doIt( [cc] + usercflags + ["-c", "-o", obj, filename] )
        doIt( [userlinker] + userlinkflags + [obj, "-o", exe ])

doIt( [python, "fool.zip", "sd.img",
    "create", "64",
    "cp", "user/hello.exe", "HELLO.EXE"
])

doIt( [ qemu,
    "-m", "512",                #memory (512MB)
    "-M", "raspi0",             #machine (raspberry pi 0)
    "-kernel", "kernel.elf",    #kernel file
    "-echr", "126",             #escape character (~)
    "-serial", "mon:stdio",      #connect serial to console
    "-drive", "file=sd.img,if=sd,format=raw"
])