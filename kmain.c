
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


void kmain(){
    bss_init();
    serial_init();
    kprintf("We the People of the United States\n");
    while(1){
    }
}