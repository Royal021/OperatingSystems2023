#include "serial.h"
#include "console.h"
#include "etec3701_10x20.h"
#include "video.h"


#pragma pack(push,1)
struct Color{
    u8 r,g,b;
};
#pragma pack(pop)


void console_putc(unsigned char ch)
{
    
    static unsigned int col = 1;
    static unsigned int row;
    //static struct Color curr_fg;
    //static struct Color curr_bg;
    static unsigned int max_col = 81;
    serial_putc(ch);    

    //are we doing color, do color
    //

    if(col>=max_col)
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
        //setflag

        return; // NEXT WEEK
    }
    else if(ch=='\x7f')
    {
        if(row ==0 && col == 1 )
            return;
        
        else if(col > 1 )
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
        col+=8-(col-1)%8;
    }
    else
    {
        video_draw_character(ch, col*CHAR_WIDTH,row*CHAR_HEIGHT);
        col+=1;
    }

    if(row ==30)
    {
         kmemcpy(framebuffer, framebuffer+pitch* CHAR_HEIGHT, (600-CHAR_HEIGHT)*pitch);
         for(int f = 0;f<80;f++)
         {
                video_draw_character(' ', col*CHAR_WIDTH,row*CHAR_HEIGHT);
         }
         col = 0;
         row = 29;
    }
    
}