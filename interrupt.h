#pragma once

#include "utils.h"
#include "kprintf.h"


#define CONTROLLER_BASE  (PERIPHERAL_BASE + 0xb000)
#define ENABLE0      ( (volatile u32*)(CONTROLLER_BASE+0x210) )
#define ENABLE1      ( (volatile u32*)(CONTROLLER_BASE+0x214) )
#define DISABLE0     ( (volatile u32*)(CONTROLLER_BASE+0x21c) )
#define DISABLE1     ( (volatile u32*)(CONTROLLER_BASE+0x220) )
#define MAX_HANDLERS 8

typedef void (*InterruptHandler)(void);

extern void* interrupt_table_begin;
extern void* interrupt_table_end;
void interrupt_init();
void halt();
void handler_reset();
void handler_undefined();
void handler_svc();
void handler_prefetch_abort();
void handler_data_abort();
void handler_reserved();
void handler_irq();
void handler_fiq();
void interrupt_enable();
void register_interrupt_handler(int irq, InterruptHandler h);

