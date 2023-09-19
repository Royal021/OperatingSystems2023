#pragma once

#define PERIPHERAL_BASE (0x20000000)
typedef unsigned char u8;
typedef unsigned u32;

void bss_init();
void kmemset(void* p, u8 val, u32 count);
void kmemcpy( void* dest, const void* src, unsigned count);
u8 getValue(u8 i, u8 color);
void panic(const char *s);
