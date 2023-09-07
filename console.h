#pragma once
#include "utils.h"
#define CHAR_WIDTH 10
#define CHAR_HEIGHT 20
#define HEIGHT 600

extern u32 pitch;
extern u8* framebuffer;

void console_putc(unsigned char ch);