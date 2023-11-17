__asm__(
    ".global _start\n"
    "_start:\n"
    "mov sp, #0x8000\n"
    "b kmain"
);

#include "serial.h"
#include "utils.h"
#include "kprintf.h"
#include "video.h"
#include "interrupt.h"
#include "timer.h"
#include "sd.h"
#include "exec.h"

void kmain(){
    bss_init();
    serial_init();
    video_init();
    interrupt_init();
    timer_init();
    interrupt_enable();
    sd_init();


    int rv = exec("HELLO.EXE");
    kprintf("exec: %d\n",rv);
    panic("Could not exec!");

        while(1){
        halt();
    }
}
