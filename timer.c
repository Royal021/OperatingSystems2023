#include "interrupt.h"
#include "utils.h"
#include "kprintf.h"


#define TIMER_BASE   (PERIPHERAL_BASE+0x3000)
#define CONTROL_STATUS  ( (volatile u32*)(TIMER_BASE+0x00) )
#define CURRENT         ( (volatile u32*)(TIMER_BASE+0x04) )
#define COMPARE         ( (volatile u32*)(TIMER_BASE+0x10) )

//one jiffy = 100 msec
#define MICROSECONDS_PER_JIFFY 100000
#define MILLISECONDS_PER_JIFFY 100
volatile u32 uptime=0;

unsigned get_uptime()
{
    return uptime*MILLISECONDS_PER_JIFFY;
}

void timer_interrupt()
{
    u32 v = *CONTROL_STATUS;
    if( v&2 ){
        //timer interrupt
        *COMPARE = *CURRENT + MICROSECONDS_PER_JIFFY;
        *CONTROL_STATUS = 2;
        uptime++;
    }
}

void timer_init()
{
    *COMPARE = *CURRENT + MICROSECONDS_PER_JIFFY;
    //1=timer irq
    register_interrupt_handler(1,timer_interrupt);
}
