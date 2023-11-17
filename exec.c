#include "file.h"
#include "utils.h"
#include "errno.h"
#include "kprintf.h"

#define EXE_STACK 0x800000

#define MAX_SECTIONS 16


#pragma pack(push,1)
struct ElfHeader{
    char magic[4];      //always 0x7f,E,L,F
    u8 class;           //1=32 bit, 2=64 bit
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
    u32 type;
    u32 offset;     //offset in file to segment start
    u32 address;    //address in memory for segment
    u32 physaddr;   //we can ignore this
    u32 sizeInFile; //size in file
    u32 sizeInRAM;  //size in memory
    u32 flags;
    u32 alignment;  //address alignment
};
#pragma pack(pop)


int exec(const char* fname)
{
    int fd = file_open(fname, O_RDONLY );
    if( fd < 0 )
        return fd;

    static struct ElfHeader elfHeader;
    int rv = file_read_fully( fd, &elfHeader, sizeof(elfHeader) );
    if(rv < 0 ){
        goto error;
    }
    if( rv != sizeof(elfHeader) ){
        rv=ENOEXEC;
        goto error;
    }


    if( 0 != kmemcmp(elfHeader.magic,"\x7f" "ELF",4) ||
        elfHeader.encoding != 1 ||
        elfHeader.type != 2 ||
        elfHeader.machine != 40 ){

        rv = ENOEXEC;
        goto error;
    }


    for(unsigned i=0;i<elfHeader.programHeaderCount;++i){
        unsigned offset = elfHeader.programHeaderOffset;
        offset += i*elfHeader.programHeaderEntrySize;
        rv = file_seek(fd,(int)offset,SEEK_SET);
        if(rv<0){
            goto error;
        }
        static struct ProgramHeaderEntry pentry;
        rv = file_read_fully( fd, &pentry, sizeof(pentry));
        if( rv < 0 )
            goto error;
        if( rv != sizeof(pentry) ){
            rv = ENOEXEC;
            goto error;
        }

        if( pentry.type != 1 ){
            continue;
        }

        unsigned amountToLoad = min( pentry.sizeInFile, pentry.sizeInRAM);

        if( amountToLoad > 0 ){
            rv = file_seek( fd, (int)pentry.offset, SEEK_SET );
            if(rv < 0 )
                goto error;
            rv = file_read_fully(fd, (void*)pentry.address, amountToLoad);
            if( rv < 0 )
                goto error;
            if( rv != amountToLoad ){
                rv = ENOEXEC;
                goto error;
            }
        }

        if( pentry.sizeInRAM > amountToLoad ){
            kmemset( (void*)(pentry.address + amountToLoad),
                     0,
                     pentry.sizeInRAM-amountToLoad
            );
        }
    }


    u32 entryPoint = elfHeader.entryPoint;
    file_close(fd);


    if( entryPoint < 0x400000 || entryPoint >= 0x800000 ){
        return ENOEXEC;
    }
    
    unsigned temp,temp2;
    
    __asm__ volatile(
        
        //switch to system mode
        "mrs %[tempReg1], cpsr              \n"
        "mov %[tempReg2], %[tempReg1]       \n"
        "orr %[tempReg1], #0x1f             \n"
        "msr cpsr, %[tempReg1]              \n"
        //set banked stack pointer and zero out link register
        "mov sp, %[stackBaseReg]            \n"
        "mov lr, #0                         \n"
        //switch back to svc mode
        "msr cpsr, %[tempReg2]              \n"
        //set spsr to usr mode
        //NOTE: This assumes we are not going to thumb mode!
        "and %[tempReg1], #0xe0             \n"
        "orr %[tempReg1], #0x10             \n"
        "msr spsr, %[tempReg1]              \n"
        //zero out gpr's
        //and set lr (entry point)
        "mov lr, %[entryPointReg]  \n"
        "and r0, #0                \n"
        "and r1, #0                \n"
        "and r2, #0                \n"
        "and r3, #0                \n"
        "and r4, #0                \n"
        "and r5, #0                \n"
        "and r6, #0                \n"
        "and r7, #0                \n"
        "and r8, #0                \n"
        "and r9, #0                \n"
        "and r10, #0               \n"
        "and r11, #0               \n"
        "and r12, #0               \n"
        //transfer control and copy spsr to cpsr
        //assumes exe does not use thumb mode
        "subs pc, lr, #0            \n"
        : [tempReg1] "+r" (temp),
          [tempReg2] "+r" (temp2),
          [entryPointReg] "+r" (entryPoint)
        : [stackBaseReg]  "r" (EXE_STACK)
        : "memory","cc" //clobbers
    );

    return -1;  //dummy

error:
    file_close(fd);
    return rv;

}
