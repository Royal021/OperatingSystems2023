#pragma once

#define PERIPHERAL_BASE (0x20000000)
#define NULL (0)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned u32;

void bss_init();
void kmemset(void* p, u8 val, u32 count);
void kmemcpy(void* dest, const void* src, unsigned count);
void halt();
void panic(const char* msg);
char toupper(char c);
int kmemcmp(const void* a, const void* b, unsigned count);
unsigned min(unsigned a, unsigned b);
