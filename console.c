#include "serial.h"
#include "console.h"
#include "etec3701_10x20.h"
#include "utils.h"
#include "video.h"


void console_putc(unsigned char ch)
{
    static unsigned int col;
    static int row;
    static int max_col = 80;
    //serial_putc(ch);    
    if (ch == '\n')
    {
        col = 0; 
        row++;
    }
    if (ch == '\r')
        col=0;
    if (ch=='\t')
        col+=8-col%8;
    if(ch == '\f')
    {
        col = 0; 
        row=0;
        kmemset(f.pointer,0,f.pitch*HEIGHT);
    }
    if(ch == '\e')
        return; // NEXT WEEK
    if(ch=='\x7f')
    {
        if(row ==0 )
            return;
        else
        {
            if(column >0)
            {
                col -=1; 
                    video_draw_character(col*CHAR_WIDTH,row*CHAR_HEIGHT,' ');
            }
            else
            {
                row-=1; 
                col = 79; 
                video_draw_character(col*CHAR_WIDTH,row*CHAR_HEIGHT,' ');
            }
        }
    }
    video_draw_character(col*CHAR_WIDTH,row*CHAR_HEIGHT,ch);
    col+=1;
    if(col>=max_col)
    {
        col=0; 
        row++;
    }
}