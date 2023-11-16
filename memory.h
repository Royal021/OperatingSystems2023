#pragma once
#include "video.h"
#include "utils.h"
#define PAGE_PRESENT            ( 1<<1 )
#define PAGE_ABSENT             ( 0  )
#define PAGE_ORDINARY_MEMORY    ( (1<<3) | (1<<2) | (1<<16) )
#define PAGE_DEVICE_MEMORY      0
#define PAGE_EXECUTABLE         0
#define PAGE_ACCESS_NONE_NONE   0
#define PAGE_ACCESS_RWX_NONE    ( (1<<10) | PAGE_EXECUTABLE )
#define PAGE_ACCESS_RWX_RX      ( (2<<10) | PAGE_EXECUTABLE )
#define PAGE_ACCESS_RWX_RWX     ( (3<<10) | PAGE_EXECUTABLE )
#define PAGE_FRAME_NUMBER(x)    ( (x) << 20 )



void memory_barrier();
void memory_init();
void innitalize_page_table(unsigned* page_table);
void page_table_enable_checks();
void set_page_table(void* p);
void invalidate_tlb();
void enable_mmu();
