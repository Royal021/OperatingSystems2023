#pragma once

//need typedef for u32
#include "utils.h"

void video_init();
void video_set_pixel( unsigned x, unsigned y, struct Pixel pix);
static void mailbox_write( u32 channel, u32 data);
static u32 mailbox_read(u32 channel);
void video_draw_character(char ch, unsigned x, unsigned y);