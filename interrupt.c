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
        "mov r0,lr\n"
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
        "add sp," STACK_SIZE_STR "\n"
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

#include "interrupt.h"
extern void* interrupt_table_begin;
extern void* interrupt_table_end;

//typedef void (*InterruptHandler)(void);
static int numHandlers=0;
static InterruptHandler handlers[MAX_HANDLERS];

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
void handler_undefined(unsigned faultingAddress)
{
    kprintf("UNDEFINED ABORT at 0x%x\n", faultingAddress);
    while(1)
        halt();
}
void handler_svc()
{
    kprintf("SVC INT\n");
    halt();
}
void handler_prefetch_abort(unsigned faultingInstruction)
{
    unsigned fault_flags;
    __asm__ volatile("mrc p15, 0, %[reg],c5,c0,1" : [reg] "=r"(fault_flags));
    fault_flags &= 0xf;     //just keep low 4 bits

    kprintf("Prefetch abort at 0x%x: ", faultingInstruction);
    switch(fault_flags){
        case 1:
        case 3:
            kprintf("Alignment error\n");
            break;
        case 5:
        case 7:
            kprintf("Page absent\n");
            break;
        case 13:
        case 15:
            kprintf("Permission problem\n");
            break;
        default:
            kprintf("Unknown problem\n");
    }
    while(1)
        halt();
}
void handler_data_abort(unsigned faultingAddress)
{
    unsigned fault_flags;
    __asm__ volatile(“mrc p15, 0, %[reg],c5,c0,0” : [reg] “=r”(fault_flags));
    fault_flags &= 0xf;     //just keep low 4 bits

    unsigned fault_address; //the address that couldn’t be accessed
    __asm__ volatile(“mrc p15,0, %[reg],c6,c0,0” : [reg] “=r” (fault_address) );

    kprintf("Prefetch abort at 0x%x: ", faultingAddress);
    switch(fault_flags){
        case 1:
        case 3:
            kprintf("Alignment error\n");
            break;
        case 5:
        case 7:
            kprintf("Page absent\n");
            break;
        case 13:
        case 15:
            kprintf("Permission problem\n");
            break;
        default:
            kprintf("Unknown problem\n");
    }
    while(1)
        halt();
}
void handler_reserved()
{
    kprintf("RESERVED\n");
    halt();
}
void handler_irq()
{
    //kprintf("IRQ\n");
    for(int i =0; i<numHandlers;i++)
    {
        //what needs to be done
        handlers[i]();
    }
}
void handler_fiq()
{
    kprintf("FIQ\n");
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
    if(numHandlers >=MAX_HANDLERS)
    {
        panic("too many interupts!");
    }
    else
    {
        handlers[numHandlers++]=h;
        if (irq<=31)
        {
            *ENABLE0 |= 1<<irq;
        }
        else
        {
            *ENABLE1 |= 1<<(irq-31);
        }
    }

}