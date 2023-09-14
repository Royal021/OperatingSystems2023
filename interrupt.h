#pragma once

#include "utils.h"
#include "kprintf.h"

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
