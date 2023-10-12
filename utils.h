#pragma once

#include "kprintf.h"
#define PERIPHERAL_BASE (0x20000000)
#define NULL ((void*)0)

typedef unsigned char u8;
typedef unsigned u32;

char toupper(char c);
void bss_init();
void kmemset(void* p, u8 val, u32 count);
void kmemcpy( void* dest, const void* src, unsigned count);
u8 getValue(u8 i, u8 color);
void panic(const char *s);
int kmemcmp(const void* a, const void* b, unsigned count);
u32 Min32(u32 X,u32 Y);