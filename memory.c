//memory.c

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

void __aeabi_memclr(void* ptr, unsigned size)
{
    char* p = (char*) ptr;
    while(size>0){
        *p=0;
        p++;
        size--;
    }
}

void __aeabi_memclr8(void* ptr, unsigned size)
{
    __aeabi_memclr(ptr,size);
}
