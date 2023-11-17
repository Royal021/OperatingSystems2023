int do_syscall(int req, unsigned p1, unsigned p2, unsigned p3){
    register unsigned r0 __asm__("r0");
    register unsigned r1 __asm__("r1");
    register unsigned r2 __asm__("r2");
    register unsigned r3 __asm__("r3");
    r0=(unsigned)req;
    r1=p1;
    r2=p2;
    r3=p3;
    //svc takes an argument, but we don't use it
    __asm__ volatile ("svc #0"
        : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3)
        :   //no inputs
        : "memory", "cc" //clobbers
    );
    return (int)r0;
}
