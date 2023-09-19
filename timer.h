#include "interrupt.h"
#include "utils.h"
#define TIMER_BASE   (PERIPHERAL_BASE+0x3000)
#define CONTROL_STATUS  ( (volatile u32*)(TIMER_BASE+0x00) )
#define CURRENT         ( (volatile u32*)(TIMER_BASE+0x04) )
#define COMPARE         ( (volatile u32*)(TIMER_BASE+0x10) )

void timer_init();
void timer_interrupt();
unsigned get_uptime();