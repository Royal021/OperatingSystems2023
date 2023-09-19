#include "timer.h"

//1MS

unsigned delta = 10000;
void timer_init()
{
    *COMPARE = *CURRENT + delta;
    register_interrupt_handler(2, timer_interrupt);

}

void timer_interrupt()
{
    v = *CONTROLLER_BASE
    if(v & 2)
    {
        *COMPARE = 0;
        //WRITE 2 TO CONTROL STATUS
        *CONTROL_STATUS |= 1<<1;
        //write/2 to control stactus to
    }
}

unsigned get_uptime()
{
    return 1;
}