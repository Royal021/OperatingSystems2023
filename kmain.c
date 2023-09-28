
__asm__(
    ".global _start\n"
    "_start:\n"
    "mov sp, #0x8000\n"
    "b kmain"
);


#include "serial.h"
#include "utils.h"
#include "console.h"
#include "kprintf.h"
#include "video.h"
#include "memory.h"
#include "interrupt.h"
#include "timer.h"
#include "sd.h"
#include "fs.h"


extern void sweet();

void kmain()
{
    bss_init();
    serial_init();
    video_init();
    interrupt_init();
    timer_init();
    interrupt_enable();
    sd_init();
    disk_init();
    char buffery[512];
    sd_read_sector(0, buffery);

    for(int i = 0; i<512; i++)
    {
        kprintf("%c", buffery[i]);
    }
    //sweet();
    serial_putc('\n');
    serial_putc('D');
    serial_putc('O');
    serial_putc('N');
    serial_putc('E');
    serial_putc('\n');


    while(1){
    }
}