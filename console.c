#include "serial.h"
#include "console.h"
#include "etec3701_10x20.h"
#include "utils.h"

static int curr_col;
static int curr_row;
static int max_col = 80;
//tab 5, new line 8
//if (c == '\n')
//col = 0; row++;
//if (c == '\r')
//col=0;
//if (c=='\t')
//col+=8-col%8;
//if(c == '\f')
//col = 0; row=0;
//kmemset(fb,0,pITCH*HEIGHT)
//if(c == '\e')
//continue; // NEXT WEEK
//if(c=='\x7f')
//{
//if(row ==0 )
//continue;
//else
//{
//if(column >0)
//col -=1; video_draw_character(' ');
//else
//row-=1; col = 79; video_draw_character(' ');
//}
//}
//video_draw_character();
//col+=1;
//if(col>=80)
//{col=0; row++;}

void console_putc(char ch)
{
    serial_putc(ch);    
}