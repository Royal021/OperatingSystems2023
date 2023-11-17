#include "utils.h"
#include "kprintf.h"

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
    while(count>0){
        *c=val;
        c++;
        count--;
    }
}

void kmemcpy(void* dest, const void* src, unsigned count)
{
    char* d = (char*) dest;
    char* s = (char*) src;
    while(count>0){
        *d=*s;
        d++;
        s++;
        count--;
    }
}

void halt(){
    __asm__ volatile(
        "mov r0,#0\n"
        "mcr p15,0,r0,c7,c0,4"
        : : : "r0"
    );
}

void panic(const char* msg){
    kprintf("\n\nPANIC: %s\n",msg);
    while(1){
        halt();
    }
}

char toupper(char c)
{
    if( c >= 'a' && c <= 'z' )
        return c-32;
    else
        return c;
}

int kmemcmp(const void* a, const void* b, unsigned count)
{
    char* aa = (char*) a;
    char* bb = (char*) b;
    while(count>0){
        if( *aa < *bb )
            return -1;
        if( *aa > *bb )
            return 1;
        aa++;
        bb++;
        count--;
    }
    return 0;
}

unsigned min(unsigned a, unsigned b)
{
    if(a<=b)
        return a;
    return b;
}
