#pragma once
#include "utils.h"
#include "etec3701_10x20.h"
#include "video.h"
#include "serial.h"
#define CHAR_WIDTH 10
#define CHAR_HEIGHT 20
#define HEIGHT 600

extern u32 pitch;
extern u8* framebuffer;

void console_putc(unsigned char ch);