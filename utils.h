#pragma once

#define PERIPHERAL_BASE (0x20000000)
typedef unsigned char u8;
typedef unsigned u32;

void bss_init();
void kmemset(void* p, u8 val, u32 count);