#pragma once

typedef void (*InterruptHandler)(void);


void interrupt_init();
void interrupt_enable();
void register_interrupt_handler(int irq, InterruptHandler h);
