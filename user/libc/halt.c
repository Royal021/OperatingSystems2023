#include "syscall.h"
void halt()
{
    do_syscall(SYSCALL_HALT, 0, 0, 0);
}
