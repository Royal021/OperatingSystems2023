#include "utils.h"

extern void* _start_bss;
extern void* _end_bss;

void bss_init()
{
    char* start = (char*)&_start_bss;
    char* end = (char*)&_end_bss;
    kmemset(start, 0, (unsigned)(end-start) );
}

void kmemset(void* p, u8 val, u32 count)
{
    u8* c = (u8*)p;
    for(u32 i=0; i<count; i++, c++)
    {
        *c = val;
    }
}

void kmemcpy( void* dest, const void* src, unsigned count)
{
    //frame
    //precondition, no overlap or src>dest
    char *d = (char*) dest;
    char *s = (char*) src;
    while(count-->0)
    {
        *d++ = *s++;
    }
}