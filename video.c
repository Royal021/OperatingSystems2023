#include "utils.h"
#include "video.h"
#include "memory.h"
#include "etec3701_10x20.h"
#include "kprintf.h"

#define VIDEO_MAILBOX  (PERIPHERAL_BASE + 0x0000b880)
#define VIDEO_STATUS   ( (volatile u32*)(VIDEO_MAILBOX+0x18) )
#define VIDEO_READ     ( (volatile u32*)(VIDEO_MAILBOX+0x0) )
#define VIDEO_WRITE    ( (volatile u32*)(VIDEO_MAILBOX+0x20) )

#define MAILBOX_FULL        ((u32)(1<<31))
#define MAILBOX_EMPTY       ((u32)(1<<30))

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

static volatile u8* framebuffer;
static u32 pitch;

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

void video_set_pixel( unsigned x, unsigned y, struct Pixel pix)
{
    struct Pixel* p = (struct Pixel*)(framebuffer + pitch*y);
    p += x;
    *p = pix;
}


void video_draw_character(char ch, unsigned x, unsigned y,
                          struct Pixel fg, struct Pixel bg)
{
    unsigned idx = (unsigned)(ch);
    idx &= 0xff;
    for(unsigned i=0;i<CHAR_HEIGHT;++i){
        unsigned mask = 1<<(CHAR_WIDTH-1);
        for(unsigned j=0;j<CHAR_WIDTH;++j,mask>>=1){
            if( font_data[idx][i] & mask ){
                video_set_pixel(x+j, y+i, fg );
            } else
                video_set_pixel(x+j, y+i, bg );
        }
    }
}

void video_clear_screen(struct Pixel color)
{
    volatile u8* p = framebuffer;
    for(int y=0;y<SCREEN_HEIGHT;++y, p+=pitch){
        struct Pixel* pix = (struct Pixel*) p;
        for(int x=0;x<SCREEN_WIDTH;++x,pix++){
            *pix = color;
        }
    }
}


void video_init()
{
    struct FBInfo finfo __attribute__((aligned(16)))= {
        .width = SCREEN_WIDTH,
        .virtualWidth = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .virtualHeight = SCREEN_HEIGHT,
        .depth = 24
    };

    u32 tmp = (u32) &finfo;
    while(1){
        mailbox_write(1, tmp>>4);
        u32 v = mailbox_read(1);
        if( v == 0 && finfo.pointer )
            break;
    }

    framebuffer = finfo.pointer;
    pitch = finfo.pitch;
}

void video_scroll(unsigned rowcount)
{
    char* d = (char*) framebuffer;
    char* s = (char*) (framebuffer + rowcount * pitch );
    unsigned num = SCREEN_WIDTH*3;
    for(unsigned y=rowcount;y<SCREEN_HEIGHT;y++){
        kmemcpy(d,s,num);
        d+=pitch;
        s+=pitch;
    }
}
