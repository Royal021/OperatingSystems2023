#include "serial.h"
#include "video.h"

#define USE_COLOR 1

#define CHAR_WIDTH 10
#define CHAR_HEIGHT 20

static int row=0;
static int col=0;


static struct Pixel fg = {
    .r=172,
    .g=172,
    .b=172
};

static struct Pixel bg = {
    .r=0,
    .g=0,
    .b=172
};

struct Pixel rgb(int x){
    struct Pixel p;
    u8 on,off;
    if( x & 8 ){
        on=255;
        off=82;
    } else {
        on=172;
        off=0;
    }

    p.r = (x&4) ? on:off;
    p.g = (x&2) ? on:off;
    p.b = (x&1) ? on:off;
    return p;
}

void console_putc(char c)
{
    static int nextIsEscape=0;

    serial_putc(c);

    if( nextIsEscape ){
        nextIsEscape=0;
        #if USE_COLOR
            int b = (c>>4) & 0xf;
            int f = c&0xf;
            bg = rgb(b);
            fg = rgb(f);
        #endif
        return;
    }

    switch(c){
        case '\n':
            row++;
            col=0;
            break;
        case '\r':
            col=0;
            break;
        case '\t':
        {
            int amt = 8 - (col%8);
            col+=amt;
            if( col >= 80 ){
                col = 0;
                row++;
            }
            break;
        }
        case '\f':
            row=0;
            col=0;
            video_clear_screen(bg);
            break;
        case '\x7f':
            col--;
            if( col<0 ){
                row--;
                col=79;
                if( row < 0 ){
                    row=0;
                    col=0;
                }
            }
            video_draw_character( ' ',
                                (unsigned)(col*CHAR_WIDTH),
                                (unsigned)(row*CHAR_HEIGHT),
                                fg,bg
            );
            break;
        case '\e':
            nextIsEscape=1;
            break;
        default:
            video_draw_character(c,
                                (unsigned)(col*CHAR_WIDTH),
                                (unsigned)(row*CHAR_HEIGHT),
                                fg,bg);
            col++;
            if( col == 80 ){
                row++;
                col=0;
            }
    }

    if( row == 30 ){
        row=29;
        col=0;
        video_scroll(CHAR_HEIGHT);
        unsigned y = (unsigned)(row*CHAR_HEIGHT);
        unsigned x=0;
        for(int i=0;i<80;++i,x+=CHAR_WIDTH){
            video_draw_character(' ', x,y, fg,bg );
        }
    }


}
