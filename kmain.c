
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
    unsigned start_X = 109;
    unsigned start_Y= 200;
    video_draw_character('W', start_X,start_Y);
    video_draw_character('e', start_X+cw, start_Y);
    video_draw_character('t', start_X+cw*3, start_Y);
    video_draw_character('h', start_X+cw*4, start_Y);
    video_draw_character('e', start_X+cw*5, start_Y);
    video_draw_character('P', start_X+cw*7, start_Y);
    video_draw_character('e', start_X+cw*8, start_Y);
    video_draw_character('o', start_X+cw*9, start_Y);
    video_draw_character('p', start_X+cw*10, start_Y);
    video_draw_character('l', start_X+cw*11, start_Y);
    video_draw_character('e', start_X+cw*12, start_Y);
    video_draw_character('o', start_X+cw*14, start_Y);
    video_draw_character('f', start_X+cw*15, start_Y);
    video_draw_character('t', start_X+cw*17, start_Y);
    video_draw_character('h', start_X+cw*18, start_Y);
    video_draw_character('e', start_X+cw*19, start_Y);
    video_draw_character('U', start_X+cw*21, start_Y);
    video_draw_character('n', start_X+cw*22, start_Y);
    video_draw_character('i', start_X+cw*23, start_Y);
    video_draw_character('t', start_X+cw*24, start_Y);
    video_draw_character('e', start_X+cw*25, start_Y);
    video_draw_character('d', start_X+cw*26, start_Y);
    video_draw_character('S', start_X+cw*28, start_Y);
    video_draw_character('t', start_X+cw*29, start_Y);
    video_draw_character('a', start_X+cw*30, start_Y);
    video_draw_character('t', start_X+cw*31, start_Y);
    video_draw_character('e', start_X+cw*32, start_Y);
    video_draw_character('s', start_X+cw*33, start_Y); 
    kprintf("\nDONE\n");
    while(1){
    }
}