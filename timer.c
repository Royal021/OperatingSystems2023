#include "timer.h"
//1MS

unsigned delta = 10000;
unsigned jiffies = 0;
void timer_init()
{
    *COMPARE = *CURRENT + delta;
    register_interrupt_handler(1, timer_interrupt);

}

void timer_interrupt(unsigned registers[])
{
    u32 v = *CONTROL_STATUS;
    if(v & 2)
    {
        *COMPARE = *CURRENT + delta;
        //WRITE 2 TO CONTROL STATUS
        *CONTROL_STATUS =2;
        jiffies+=1;
        schedule(registers); //defined in sched.c
    }
}

unsigned get_uptime()
{
    return jiffies * delta/1000;
}