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