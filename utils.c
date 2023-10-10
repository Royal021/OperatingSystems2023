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


u8 getValue(u8 i, u8 color)
{

    if(!color && !i)
        return 0;
    if(!color && i)
        return 82;
    if(color && !i)
        return 172;
    else
        return 255;
}

void panic(const char *s)
{
    kprintf("PANIC PANIC PANIC AT THE DISCO\n");
    while(1){;}
}

char toupper(char c)
{
    if( c>= 'a' && c <= 'z')
    {
        c-=32;
    }
    return c;
}

//compare blocks of memory pointed to by a and b , look at count bytes
//return negative if a<b, zero if a == b, positive if a>b
int kmemcmp(const void* a, const void* b, unsigned count)
{
    char *ap = (char*) a;
    char *bp = (char*) b;
    while(count>0){
        if(*ap<*bp)
            return -1;
        if(*ap>*bp)
            return 1;
        --count;
        ap++;
        bp++;
    }
    return 0;
}