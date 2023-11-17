#include "errno.h"
#include "syscalldefs.h"
#include "kprintf.h"
#include "utils.h"
#include "file.h"
#include "console.h"

int syscall_handler(int req, unsigned p1, unsigned p2, unsigned p3)
{
     switch(req){
        case SYSCALL_TEST:
            kprintf("Syscall test! %d %d %d\n",
                p1,p2,p3
            );
            return (int)(p1+p2+p3);
        case SYSCALL_HALT:
            halt();
            return SUCCESS;
        case SYSCALL_READ:
        {
            int fd = (int)p1;
            if( fd == 0 )
                return ENOSYS;
            if( fd == 1 || fd == 2 )
                return EINVAL;
            return file_read(fd,(void*)p2, p3 );
        }
        case SYSCALL_WRITE:
        {
            int fd = (int)p1;
            if( fd == 0 )
                return EINVAL;
            if( fd == 1 || fd == 2 ){
                char* c = (char*) p2;
                for(unsigned i=0;i<p3;++i){
                    console_putc(c[i]);
                }
                return (int)p3;
            }
            return file_write(fd,(void*)p2, p3 );
        }
        default:
            return ENOSYS;
    }

}
