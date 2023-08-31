#include "utils.h"
#include "video.h"
#include "etec3701_10x20.h"
#include "memory.h"

#define VIDEO_MAILBOX  (PERIPHERAL_BASE + 0x0000b880)
#define VIDEO_STATUS   ( (volatile u32*)(VIDEO_MAILBOX+0x18) )
#define VIDEO_READ     ( (volatile u32*)(VIDEO_MAILBOX+0x0) )
#define VIDEO_WRITE    ( (volatile u32*)(VIDEO_MAILBOX+0x20) )

#define MAILBOX_FULL        ((u32)(1<<31))
#define MAILBOX_EMPTY       ((u32)(1<<30))

#define WIDTH 800
#define HEIGHT 600

#pragma pack(push,1)
struct FBInfo{
    u32 width __attribute__((aligned(16)));
    u32 height;
    u32 virtualWidth;
    u32 virtualHeight;
    u32 pitch;      //output; initialize to zero
    u32 depth;      //24 should work; 16 might work
    u32 xoffset;    //zero
    u32 yoffset;    //zero
    u8* pointer;    //output; initialize to zero
    u32 size;       //output; initialize to zero
};
#pragma pack(pop)

#pragma pack(push,1)
struct Pixel{
    u8 r,g,b;
};
#pragma pack(pop)


static void mailbox_write( u32 channel, u32 data)
{
    while(1){
        memory_barrier();
        u32 tmp = *VIDEO_STATUS;
        if( 0 == ( tmp & MAILBOX_FULL ) )
            break;
    }
    *VIDEO_WRITE = (channel) | (data<<4);
}

static u32 mailbox_read(u32 channel){
    while(1){
        memory_barrier();
        u32 tmp = *VIDEO_STATUS;
        if( 0 == ( tmp & MAILBOX_EMPTY) ){
            tmp = *VIDEO_READ;
            u32 ch = tmp & 0xf;
            if( channel == ch ){
                return tmp >> 4;
            }
        }
    }
}


static volatile u8* framebuffer;
static u32 pitch;

struct Pixel foregroundColor;
struct Pixel backgroundColor;

void video_init()
{
    struct FBInfo f __attribute__((aligned(16)));
    f.width = WIDTH; 
    f.height = HEIGHT;
    f.virtualWidth = f.width;
    f.virtualHeight = f.height;
    f.depth = 24;
    f.pitch = 90;
    f.xoffset = 0;
    f.yoffset = 0;
    f.pointer = 0;
    f.size = 0;

    foregroundColor.r = 255;
    foregroundColor.g = 255;
    foregroundColor.b = 255;

    backgroundColor.r = 1;
    backgroundColor.g = 1;
    backgroundColor.b = 1;

    int running = 1;
    while(running)
    {
        unsigned write_address = (unsigned)&f;
        
        mailbox_write(1, write_address>>4);
        if(mailbox_read(1) ==0 && f.pointer != 0)
        {
            running=0;
        }
    }
    framebuffer = ( u8*) f.pointer;
    pitch = f.pitch;
} 


void video_set_pixel( unsigned x, unsigned y, struct Pixel pix)
{
    if(x >= WIDTH)
        return;
    if(y >= HEIGHT)
        return;

    // go to start of row that we are drawing into
    volatile struct Pixel* row = (volatile struct Pixel*)(framebuffer + y * pitch);
    
    row[x] = pix;
}

//return true if bit ii is 1 in value x
#define IS_BIT_SET(x,i) ((1<<i) & x)

//IS_BIT_SET(font_data[ch][row], col)




void video_draw_character(unsigned char ch, unsigned x, unsigned y)
{
    for(unsigned row=0; row<CHAR_HEIGHT;++row)
    {
        for(unsigned col=CHAR_WIDTH; 0<col;--col)
        {
            unsigned xx = x-col;
            unsigned yy = y+row;
            
            video_set_pixel(xx,yy, 
                IS_BIT_SET(font_data[ch][row],col) ?
                    foregroundColor : backgroundColor
            );
        }
    }
}