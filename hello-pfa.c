

#define FLAGS ( (volatile unsigned*)(0x20201018) )
#define DATA  ( (volatile unsigned*)(0x20201000) )
#define FLAG_TFIFO_FULL             (1<<5)

void my_putc(char ch)
{
    while( (*FLAGS & FLAG_TFIFO_FULL) != 0 )
        ;
    *DATA = ch;
}

void my_halt(){
    __asm__ volatile(
        "mov r0,#0\n"
        "mcr p15,0,r0,c7,c0,4"
        : : : "r0"
    );
}



int main(int argc, char* argv[])
{
    const char* msg = "Hello!\n";

    for(int i=0;msg[i];++i){
        my_putc(msg[i]);
    }

    while(1){
        //!!
        __asm__ volatile( "bkpt" );
        //!!
    }
    return 0;
}
