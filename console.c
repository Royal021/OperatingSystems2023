#include "serial.h"
#include "console.h"
#include "etec3701_10x20.h"
#include "utils.h"
#include "video.h"


extern volatile u8* framebuffer;
extern u32 pitch;

void console_putc(unsigned char ch)
{
    
    static unsigned int col;
    static unsigned int row;
    static unsigned int max_col = 80;
    serial_putc(ch);    

    if(col>=max_col)
    {
        col=0; 
        row++;
    }
    if (ch == '\n')
    {
        row++;
        col = 0; 
        
    }
    else if (ch == '\r')
    {
        col=0;
    }
    else if(ch == '\f')
    {
        col = 0; 
        row=0;
        //kmemset(framebuffer,0,pitch*HEIGHT);
    }
    else if(ch == '\e')
    {
        return; // NEXT WEEK
    }
    else if(ch=='\x7f')
    {
        if(row ==0 && col == 0 )
            return;
        
        else if(col > 0 )
        {
            col -=1; 
            video_draw_character(' ',col*CHAR_WIDTH,row*CHAR_HEIGHT);  
        }
        else
        {
            row-=1; 
            col = 80; 
            video_draw_character(' ',col*CHAR_WIDTH,row*CHAR_HEIGHT);
        }
    }
    else if (ch=='\t')
    {
        col+=8-col%8;
    }
    else
    {
        video_draw_character(ch, col*CHAR_WIDTH,row*CHAR_HEIGHT);
        col+=1;
    }
    
}