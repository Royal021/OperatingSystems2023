#define STACK_SIZE 4096
#define STACK_SIZE_STR "4096"
char reset_stack[STACK_SIZE];
char undefined_stack[STACK_SIZE];
char svc_stack[STACK_SIZE];
char prefetch_abort_stack[STACK_SIZE];
char data_abort_stack[STACK_SIZE];
char reserved_stack[STACK_SIZE];
char irq_stack[STACK_SIZE];
char fiq_stack[STACK_SIZE];

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
        "ldr sp, =reset_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #4\n"
        "push {r0-r12,lr}\n"
        "bl handler_reset\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
    "asm_handler_undefined:\n"
        "ldr sp, =undefined_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #4\n"
        "push {r0-r12,lr}\n"
        "bl handler_undefined\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
    "asm_handler_svc:\n"
        "ldr sp, =svc_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "push {r0-r12,lr}\n"
        "bl handler_svc\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
    "asm_handler_prefetch_abort:\n"
        "ldr sp, =prefetch_abort_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #4\n"
        "push {r0-r12,lr}\n"
        "bl handler_prefetch_abort\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
    "asm_handler_data_abort:\n"
        "ldr sp, =data_abort_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #8\n"
        "push {r0-r12,lr}\n"
        "bl handler_data_abort\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
    "asm_handler_reserved:\n"
        "ldr sp, =reserved_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #4\n"
        "push {r0-r12,lr}\n"
        "bl handler_reserved\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
    "asm_handler_irq:\n"
        "ldr sp, =irq_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #4\n"
        "push {r0-r12,lr}\n"
        "bl handler_irq\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
    "asm_handler_fiq:\n"
        "ldr sp, =fiq_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #4\n"
        "push {r0-r12,lr}\n"
        "bl handler_fiq\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"
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

void halt(){
    __asm__ volatile(
        "mov r0,#0\n"
        "mcr p15,0,r0,c7,c0,4"
        : : : "r0"
    );
}

void handler_reset()
{
    kprintf("RESET\n");
    halt();
}
void handler_undefined()
{
    kprintf("UNDEFINED OPCODE\n");
    halt();
}
void handler_svc()
{
    kprintf("SVC INT\n");
    halt();
}
void handler_prefetch_abort()
{
    kprintf("PREFETCH ABORT\n");
    halt();
}
void handler_data_abort()
{
    kprintf("DATA ABORT\n");
    halt();
}
void handler_reserved()
{
    kprintf("RESERVED\n");
    halt();
}
void handler_irq()
{
    kprintf("IRQ\n");
    halt();
}
void handler_fiq()
{
    kprintf("FIQ\n");
    halt();
}