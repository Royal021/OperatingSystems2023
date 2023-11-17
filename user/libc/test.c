#include "syscall.h"
int test()
{
    return do_syscall(SYSCALL_TEST, 10, 20, 30);
}
