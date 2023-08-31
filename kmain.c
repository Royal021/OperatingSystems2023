
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




void kmain(){
    bss_init();
    serial_init();
    video_init();
    unsigned cw = 10;
    video_draw_character('W', 100,200);
    video_draw_character('e', 100+cw, 200);
    video_draw_character('t', 100+cw*3, 200);
    video_draw_character('h', 100+cw*4, 200);
    video_draw_character('e', 100+cw*5, 200);
    video_draw_character('P', 100+cw*7, 200);
    video_draw_character('e', 100+cw*8, 200);
    video_draw_character('o', 100+cw*9, 200);
    video_draw_character('p', 100+cw*10, 200);
    video_draw_character('l', 100+cw*11, 200);
    video_draw_character('e', 100+cw*12, 200);
    video_draw_character('o', 100+cw*14, 200);
    video_draw_character('f', 100+cw*15, 200);
    video_draw_character('t', 100+cw*17, 200);
    video_draw_character('h', 100+cw*18, 200);
    video_draw_character('e', 100+cw*19, 200);
    video_draw_character('U', 100+cw*21, 200);
    video_draw_character('n', 100+cw*22, 200);
    video_draw_character('i', 100+cw*23, 200);
    video_draw_character('t', 100+cw*24, 200);
    video_draw_character('e', 100+cw*25, 200);
    video_draw_character('d', 100+cw*26, 200);
    video_draw_character('S', 100+cw*28, 200);
    video_draw_character('t', 100+cw*29, 200);
    video_draw_character('a', 100+cw*30, 200);
    video_draw_character('t', 100+cw*31, 200);
    video_draw_character('e', 100+cw*32, 200);
    video_draw_character('s', 100+cw*33, 200); 
    kprintf("\n DONE\n");
    while(1){
    }
}