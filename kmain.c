
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


extern void sweet();

void kmain()
{
    bss_init();
    serial_init();
    video_init();
    sweet();
    kprintf("\nDONE\n");
    while(1){
    }
}