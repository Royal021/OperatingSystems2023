#include "serial.h"
#include "console.h"

void console_putc(char ch)
{
    serial_putc(ch);    
}