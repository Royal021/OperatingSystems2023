#include "ELF.h"

int exec(const char* fname)
{
    int fd = file_open(fname, O_RDONLY );
    if( fd < 0 )
        return fd;

    static struct ElfHeader elfHeader;
    int rv = file_read_fully( fd, &elfHeader, sizeof(elfHeader) );
    if(rv < 0 )
    {
        goto error;
    }
    if( rv != sizeof(elfHeader) )
    {
        rv = ENOEXEC;       //not an executable
        goto error;
    }
    
    if( 0 != kmemcmp(elfHeader.magic,"\x7f" "ELF",4) ||
    elfHeader.encoding != 1 ||
    elfHeader.type != 2 ||
    elfHeader.machine != 40 )
    {
        rv = ENOEXEC;
        goto error;
    }

    for(unsigned i=0;i<elfHeader.programHeaderCount;++i){
    unsigned offset = elfHeader.programHeaderOffset;
    offset += i*elfHeader.programHeaderEntrySize;
    rv = (int)file_seek(fd,(int)offset,SEEK_SET);
    if(rv<0){
        goto error;
    }

    static struct ProgramHeaderEntry pentry;
    rv = file_read_fully( fd, &pentry, sizeof(pentry));
    if( rv < 0 )
        goto error;
    if( rv != sizeof(pentry) )
    {
        rv = ENOEXEC;
        goto error;
    }
    if( pentry.type != 1 )
    continue;

    if ( pentry.sizeInRAM >= pentry.sizeInFile )
    {
        rv = (int)file_seek(fd,(int)pentry.offset,SEEK_SET);
        file_read_fully(fd, (void*)pentry.address, pentry.sizeInFile);
        kmemset((void*)(pentry.address+pentry.sizeInFile), 0, pentry.sizeInRAM-pentry.sizeInFile);
    }
    if ( pentry.sizeInFile > pentry.sizeInRAM)
    {
        rv = (int)file_seek(fd,(int)pentry.offset,SEEK_SET);
        file_read_fully(fd, (void*)pentry.address, pentry.sizeInRAM);
    }
    if (pentry.sizeInFile == 0)
    {
        kmemset((void*)pentry.address, 0, pentry.sizeInRAM);
    }
    }
    
    u32 entryPoint = elfHeader.entryPoint;
    file_close(fd);
    if( entryPoint < 0x400000 || entryPoint >= 0x800000 )
    {
        return ENOEXEC;
    }

    ///stack pointer and reset entry point



    __asm__ volatile(
    "mov sp, %[stackBaseReg]\n"
    "bx %[entryPointReg]\n"
    :   //no outputs
    : [entryPointReg] "r" (entryPoint),
      [stackBaseReg]  "r" (EXE_STACK)
    );
    error:
    file_close(fd);
    return rv;


}

int file_read_fully(int fd, void* buf, unsigned count)
{
    unsigned numRead = 0;
    char *p = (char*) buf;
    while(count>0)
    {
        int rv = file_read(fd, p, count);
        if(rv<0)
        {
            return rv;
        }
        if(rv == 0)
        {
            break;
        }
        count -= (unsigned)rv;
        p+=(unsigned)rv;
        numRead+=(unsigned)rv;
    }
    
    return (int)numRead;

}