
#include "libc/printf.h"
#include "libc/halt.h"
#include "libc/test.h"

int main(int argc, char* argv[])
{
    //!!
    printf("Hello!\n");
    //!!

    char* x = (char*)(16*1024*1024);
    *x = 42;
    while(1){
        halt();
    }
    return 0;
}
