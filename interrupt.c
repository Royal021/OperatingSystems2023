#include "interrupt.h"
#include "utils.h"
#include "syscall.h"


#define CONTROLLER_BASE  (PERIPHERAL_BASE + 0xb000)
#define ENABLE0      ( (volatile u32*)(CONTROLLER_BASE+0x210) )
#define ENABLE1      ( (volatile u32*)(CONTROLLER_BASE+0x214) )
#define DISABLE0     ( (volatile u32*)(CONTROLLER_BASE+0x21c) )
#define DISABLE1     ( (volatile u32*)(CONTROLLER_BASE+0x220) )


#define STACK_SIZE 4096
#define STACK_SIZE_STR "4096"
char reset_stack[STACK_SIZE];
char undefined_stack[STACK_SIZE];
char svc_stack[STACK_SIZE];
char prefetch_abort_stack[STACK_SIZE];
char data_abort_stack[STACK_SIZE];
char irq_stack[STACK_SIZE];
char fiq_stack[STACK_SIZE];
char reserved_stack[STACK_SIZE];

#define MAX_HANDLERS 8
static int numHandlers=0;
static InterruptHandler handlers[MAX_HANDLERS];

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
        "mov r0,lr\n"
        "bl handler_undefined\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"

    "asm_handler_svc:\n"
        "ldr sp, =svc_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "push {r1-r12,lr}\n"
        "bl handler_svc\n"
        "pop {r1-r12,lr}\n"
        "subs pc,lr,#0\n"

    "asm_handler_prefetch_abort:\n"
        "ldr sp, =prefetch_abort_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #4\n"
        "push {r0-r12,lr}\n"
        "mov r0,lr\n"
        "bl handler_prefetch_abort\n"
        "pop {r0-r12,lr}\n"
        "subs pc,lr,#0\n"

    "asm_handler_data_abort:\n"
        "ldr sp, =data_abort_stack\n"
        "add sp, " STACK_SIZE_STR "\n"
        "sub lr, #8\n"
        "push {r0-r12,lr}\n"
        "mov r0,lr\n"
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


#include "utils.h"
#include "kprintf.h"

extern void* interrupt_table_begin;
extern void* interrupt_table_end;
void interrupt_init(){
    unsigned start = (unsigned) &interrupt_table_begin;
    unsigned end = (unsigned) &interrupt_table_end;
    unsigned numBytes = end-start;
    kmemcpy((void*)0, &interrupt_table_begin, numBytes );
}

void handler_reset()
{
    kprintf("RESET\n");
    halt();
}

void handler_undefined(unsigned faultingAddress)
{
    kprintf("UNDEFINED OPCODE at 0x%x\n",faultingAddress);
    while(1)
        halt();
}

void handler_prefetch_abort(unsigned faultingAddress)
{
    kprintf("PREFETCH ABORT at 0x%x\n",faultingAddress);
    while(1)
        halt();
}

void handler_data_abort(unsigned faultingAddress)
{
    kprintf("DATA ABORT at 0x%x\n",faultingAddress);
    while(1)
        halt();
}

void handler_irq()
{
    for(int i=0;i<numHandlers;++i){
        handlers[i]();
    }
}

void handler_fiq()
{
    kprintf("FIQ\n");
    halt();
}

int handler_svc(int req, unsigned p1, unsigned p2, unsigned p3)
{
    return syscall_handler(req,p1,p2,p3);
}

void handler_reserved()
{
    kprintf("UNUSED\n");
    halt();
}


void interrupt_enable()
{
      __asm__ volatile(
        "mrs r0,cpsr\n"
        "and r0,r0,#0xffffff7f\n"
        "msr cpsr,r0"
        : : : "r0"
    );
}


void register_interrupt_handler(int irq, InterruptHandler h)
{
    if( numHandlers == MAX_HANDLERS )
        panic("Too many handlers");
    handlers[numHandlers++] = h;
    if( irq < 32 )
        *ENABLE0 = (1<<irq);
    else
        *ENABLE1 = (1<<(irq-32));
}
