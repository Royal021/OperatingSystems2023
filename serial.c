//serial.c

#include "serial.h"
#include "utils.h"


#define UART_BASE   (PERIPHERAL_BASE + 0x00201000)
#define CONTROL   ( (volatile u32*)(UART_BASE+0x30) )

#define CONTROL_ENABLE_TRANSMIT     (1<<8)
#define CONTROL_ENABLE_RECEIVE      (1<<9)
#define CONTROL_ENABLE_UART         (1)

#define LINE_CONTROL   ( (volatile u32*)(UART_BASE+0x2c) )
#define LINE_CONTROL_ENABLE_FIFO    (1<<4)
#define LINE_CONTROL_8_BIT          (3<<5)

#define INTEGER_BAUD_RATE           ( (volatile u32*)(UART_BASE+0x24) )
#define FRACTIONAL_BAUD_RATE        ( (volatile u32*)(UART_BASE+0x28) )

#define DATA  ( (volatile u32*)(UART_BASE+0x00) )
#define FLAGS ( (volatile u32*)(UART_BASE+0x18) )

//transmit and receive FIFO's
#define FLAG_TFIFO_FULL             (1<<5)
#define FLAG_RFIFO_EMPTY            (1<<4)


void serial_init()
{
    //disable uart so we can configure it
    *CONTROL = 0;

    //flush the transmit fifo
    *LINE_CONTROL = 0;

    *INTEGER_BAUD_RATE=1;
    //0.6276041666666667 * 64 = 40
    *FRACTIONAL_BAUD_RATE=40;

    *LINE_CONTROL = LINE_CONTROL_ENABLE_FIFO | LINE_CONTROL_8_BIT;

    //enable UART
    *CONTROL = CONTROL_ENABLE_UART
             | CONTROL_ENABLE_RECEIVE
             | CONTROL_ENABLE_TRANSMIT;

}

void serial_putc(char ch)
{
    while((*FLAGS & FLAG_TFIFO_FULL) != 0)
    {
    }

    *DATA = ch;


    //check FLAGS register
    //wait until FLAG_TFIFO_FULL is zero
    //Then write character to DATA register
    //kprintf("We the People of the United States")
}