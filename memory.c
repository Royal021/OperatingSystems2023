#include "memory.h"

static unsigned kernelPageTable[4096] __attribute__((aligned(16384)));

void memory_barrier()
{
    __asm__ volatile (
        "mcr p15,0,%[zero],c7,c10,5\n"   //data memory barrier
        "mcr p15,0,%[zero],c7,c5,4\n"  //instruction sync barrier
        "mcr p15,0,%[zero],c7,c10,4\n"  //data sync barrier
        :                   //no outputs
        : [zero] "r"(0)     //inputs
    );
}

void memory_init(){
    innitalize_page_table(kernelPageTable);
    set_page_table(kernelPageTable);
    page_table_enable_checks();
    invalidate_tlb();
    enable_mmu();
    return;
}

void innitalize_page_table(unsigned* page_table)
{    
    


    for(unsigned i=0; i<4096; i++)
    {
        if(i>=0 && i<=3)
        {
            page_table[i] = PAGE_ACCESS_RWX_NONE | PAGE_PRESENT | PAGE_ORDINARY_MEMORY | PAGE_FRAME_NUMBER(i);
        }
        
        else
        {
            return;
        }
    }

    //u32 x = (u32) video_get_framebuffer();
    // x /= 1024;
    // x /= 1024;
    //page_table[x] = PAGE_ACCESS_RWX_RWX | PAGE_PRESENT | PAGE_ORDINARY_MEMORY | PAGE_FRAME_NUMBER(x); 
    // u32 np = get_framebuffer_size()/1024/1024;
    //if(get_framebuffer_size() % (1024*1024))
    //    np++;
    // for(u32 j = 0; j<np; j++)
    //{
    //    page_table[x+j] = PAGE_ACCESS_RWX_RWX | PAGE_PRESENT | PAGE_ORDINARY_MEMORY | PAGE_FRAME_NUMBER(x+j);
    //}
}


void page_table_enable_checks(){
    __asm__ volatile(
        "mcr p15,0,%[reg],c3,c0,0"
        :
        : [reg] "r" (0x55555555)
        : "memory"
    );
}

void set_page_table(void* p){
    __asm__ volatile(
        "mcr p15, 0, %[reg], c2, c0, 0"
        :
        : [reg] "r"(p)
    );
    invalidate_tlb();
}

void invalidate_tlb(){
    __asm__ volatile(
        "mcr p15,0,%[reg],c8,c5,0\n"  //instruction tlb
        "mcr p15,0,%[reg],c8,c6,0\n"  //data tlb
        "mcr p15,0,%[reg],c8,c7,0\n"  //unified tlb
        :  : [reg]"r"(0) );
}

void enable_mmu(){
    __asm__ volatile(
        "mrc p15,0,r0,c1,c0,0\n"
        "orr r0,r0,#1\n"
        "mcr p15,0,r0,c1,c0,0\n"
        :   //no inputs
        :   //no outputs
        : "r0", "memory" //clobbers
    );
    kprintf("MMU active\n");
}
