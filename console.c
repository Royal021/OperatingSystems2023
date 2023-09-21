#include "console.h"

int doingColor = 0;

void console_putc(unsigned char ch)
{
    
    static unsigned int col = 1;
    static unsigned int row;
    static struct Color curr_fg;
    static struct Color curr_bg;
    static unsigned int max_col = 81;
    serial_putc(ch);    


    if(doingColor)
    {
        curr_fg.i = ch & 8;
        curr_fg.r = getValue(curr_fg.i, ch & 4);
        curr_fg.g = getValue(curr_fg.i, ch & 2);
        curr_fg.b = getValue(curr_fg.i, ch & 1);
        curr_bg.i = ch & 128;
        curr_bg.r = getValue(curr_bg.i, ch & 64);
        curr_bg.g = getValue(curr_bg.i, ch & 32);
        curr_bg.b = getValue(curr_bg.i, ch & 16);

        doingColor = 0;  
        return;
    }

    if(col==max_col)
    {
        col=1; 
        row++;
    }
    if (ch == '\n')
    {
        row++;
        col = 1; 
    }
    else if (ch == '\r')
    {
        col=1;
    }
    else if(ch == '\f')
    {
        col = 1; 
        row=0;
        kmemset(framebuffer,0,pitch*HEIGHT);
    }
    else if(ch == '\e')
    {
        doingColor = 1;
        return; 
    }
    else if(ch=='\x7f')
    {
        if(row ==0 && col == 1 )
            return;
        
        else if(col > 1 )
        {
            col -=1; 
            video_draw_character(' ',col*CHAR_WIDTH,row*CHAR_HEIGHT, curr_fg,curr_bg);  
        }
        else
        {
            row-=1; 
            col = 80; 
            video_draw_character(' ',col*CHAR_WIDTH,row*CHAR_HEIGHT, curr_fg,curr_bg);
        }
    }
    else if (ch=='\t')
    {
        col+=8-(col-1)%8;
    }
    else
    {
        video_draw_character(ch, col*CHAR_WIDTH,row*CHAR_HEIGHT, curr_fg,curr_bg);
        col++;
    }
    if(col>=max_col)
        {
            row++;
            col=1; 
        }

    if(row ==30)
    {
        row = 29;
         kmemcpy(framebuffer, framebuffer+pitch* CHAR_HEIGHT, (600-CHAR_HEIGHT)*pitch);
         for(int f = 0;f<80;f++)
         {
                video_draw_character(' ', col*CHAR_WIDTH,row*CHAR_HEIGHT, curr_fg,curr_bg);
                col++;
         }
         col = 1;
        // row = 29;
    }
    
}