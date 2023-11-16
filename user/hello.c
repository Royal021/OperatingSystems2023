
#include "libc/printf.h"
#include "libc/halt.h"
#include "libc/test.h"


int main(int argc, char* argv[])
{
    //!!
    printf("Hello!\n");
    //!!
    while(1){
        halt();
    }
    return 0;
}
