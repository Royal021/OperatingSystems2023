#include "timer.h"
//1MS

unsigned delta = 10000;
unsigned jiffies = 0;
void timer_init()
{
    *COMPARE = *CURRENT + delta;
    register_interrupt_handler(1, timer_interrupt);

}

void timer_interrupt()
{
    u32 v = *CONTROL_STATUS;
    if(v & 2)
    {
        *COMPARE = 0;
        //WRITE 2 TO CONTROL STATUS
        *CONTROL_STATUS |= 1<<1;
        jiffies+=1;

    }
}

unsigned get_uptime()
{
    return jiffies * delta;
}