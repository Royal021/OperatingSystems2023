#pragma once

//need typedef for u32
#include "utils.h"


#pragma pack(push,1)
struct Pixel{
    u8 r,g,b;
};
#pragma pack(pop)

void video_init();
void video_draw_character(char ch, unsigned x, unsigned y,
                          struct Pixel fg, struct Pixel bg);
void video_clear_screen(struct Pixel color);
void video_scroll(unsigned rowcount);
