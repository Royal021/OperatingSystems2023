
#include "libc/printf.h"
#include "libc/halt.h"
#include "libc/test.h"

int main(int argc, char* argv[])
{
    //!!
    printf("Hello!\n");
    //!!
    printf("Calling syscall test...\n");
    int rv = test();
    printf("Returned: %d\n",rv);
    while(1){
        halt();
    }
    return 0;
}
