
#include "libc/printf.h"
#include "libc/halt.h"
#include "libc/test.h"

typedef void (*FuncType)(void);

int main(int argc, char* argv[])
{
    //!!
    printf("Hello!\n");
    //!!

    FuncType f;
    //700MB: Page is absent
    f = (FuncType) (700*1024*1024);
    f();
    while(1){
        halt();
    }
    return 0;
}
