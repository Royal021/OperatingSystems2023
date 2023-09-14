#ifndef INTERRUPT_TEST
#define INTERRUPT_TEST 0
#endif

#include "kprintf.h"

void sweet()
{
    #if INTERRUPT_TEST == 0
        kprintf("\nNo interrupt\n");
    #elif INTERRUPT_TEST == 1
        kprintf("\nPrefetch abort test\n");
        __asm__ volatile("bkpt");
    #elif INTERRUPT_TEST == 2
        kprintf("\nUndefined test\n");
        __asm__ volatile(".word 0xf7f0a000");
    #elif INTERRUPT_TEST == 3
        kprintf("SVC test\n");
        __asm__ volatile("svc #1");
    #endif
}
