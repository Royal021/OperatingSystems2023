#pragma once
#include "utils.h"
#include "etec3701_10x20.h"
#include "memory.h"
typedef unsigned char u8;
struct Color{
    u8 r,g,b,i;
};

void video_init();
void video_draw_character(unsigned char ch, unsigned x, unsigned y, struct Color fg, struct Color bg );
void* video_get_framebuffer();
unsigned video_get_framebuffer_size();