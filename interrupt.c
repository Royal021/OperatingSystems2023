__asm__ (
    ".ltorg\n"
    "interrupt_table_begin:\n"
    "ldr pc, =asm_handler_reset\n"
    "ldr pc, =asm_handler_undefined\n"
    "ldr pc, =asm_handler_svc\n"
    "ldr pc, =asm_handler_prefetch_abort\n"
    "ldr pc, =asm_handler_data_abort\n"
    "ldr pc, =asm_handler_reserved\n"
    "ldr pc, =asm_handler_irq\n"
    "ldr pc, =asm_handler_fiq\n"
    ".ltorg\n"
    "interrupt_table_end:\n"

    "asm_handler_reset:\n"
        "mov r0, #0xab\n"
        "ldr pc, =asm_handler_reset\n"
    "asm_handler_undefined:\n"
        "mov r0, #0xcd\n"
        "ldr pc, =asm_handler_undefined\n"
    "asm_handler_svc:\n"
        "mov r0, #0xef\n"
        "ldr pc, =asm_handler_svc\n"
    "asm_handler_prefetch_abort:\n"
        "mov r0, #0xba\n"
        "ldr pc, =asm_handler_prefetch_abort\n"
    "asm_handler_data_abort:\n"
        "mov r0, #0xdc\n"
        "ldr pc, =asm_handler_data_abort\n"
    "asm_handler_reserved:\n"
        "mov r0, #0xfe\n"
        "ldr pc, =asm_handler_reserved\n"
    "asm_handler_irq:\n"
        "mov r0, #0xaf\n"
        "ldr pc, =asm_handler_irq\n"
    "asm_handler_fiq:\n"
        "mov r0, #0xfa\n"
        "ldr pc, =asm_handler_fiq\n"
);

#include "interrupt.h"
extern void* interrupt_table_begin;
extern void* interrupt_table_end;
void interrupt_init(){
    unsigned start = (unsigned) &interrupt_table_begin;
    unsigned end = (unsigned) &interrupt_table_end;
    unsigned numBytes = end-start;
    kmemcpy((void*)0, &interrupt_table_begin, numBytes );
}
