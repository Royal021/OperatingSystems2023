__asm__(
    ".global _start\n"
    "_start:\n"
    "mov sp, #0x8000\n"
    "b kmain"
);

#include "serial.h"
#include "utils.h"


void kmain(){
    bss_init();
    serial_init();
    serial_putc('H');
    serial_putc('e');
    serial_putc('l');
    serial_putc('l');
    serial_putc('o');
    serial_putc('\n');
    while(1){
    }
}