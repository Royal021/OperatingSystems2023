
#pragma once
#include "file.h"

typedef unsigned char u8;
typedef unsigned u32;
typedef unsigned short u16;
#define SEEK_SET 0
#define EXE_STACK 0x800000

#pragma pack(push,1)
struct ElfHeader{
    char magic[4];      //always 0x7f,E,L,F
    u8 klass;           //1=32 bit, 2=64 bit
    u8 encoding;        //1=little endian, 2=big endian
    u8 fileVersion;     //always 1
    u8 abi;             //3=linux
    u8 reserved[8];
    u16 type;           //2=executable
    u16 machine;        //3=x86, 40=ARM
    u32 elfVersion;     //always 1
    u32 entryPoint;     //where to begin execution
    u32 programHeaderOffset;    //offset to program header
    u32 sectionHeaderOffset;    //offset to section header
    u32 flags;                  //we can ignore
    u16 elfHeaderSize;          //size of this header
    u16 programHeaderEntrySize; //size of one PH entry
    u16 programHeaderCount;     //number of PH entries
    u16 sectionHeaderEntrySize; //size of one SH entry
    u16 sectionHeaderCount;     //number of SH entries
    u16 stringTableIndex;       //index of string table
};
#pragma pack(pop)


#pragma pack(push,1)
struct ProgramHeaderEntry{
    u32 type;       //next slide
    u32 offset;     //offset in file to segment start
    u32 address;    //address in memory for segment
    u32 physaddr;   //we can ignore this
    u32 sizeInFile; //size in file
    u32 sizeInRAM;  //size in memory
    u32 flags;      //next slide
    u32 alignment;  //address alignment
};
#pragma pack(pop)

int exec(const char* fname);
int file_read_fully(int fd, void* buf, unsigned count);