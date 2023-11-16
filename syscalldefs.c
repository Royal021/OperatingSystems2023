//System Call Stuff, new file
#include "syscalldefs.h"
#include "errno.h"
#include "kprintf.h"
#include "console.h"
#include "file.h"

//Slide 29 version, need to change switch-case things

int syscall_handler(int req, unsigned p1,
                     unsigned p2, unsigned p3)
{
    switch(req){
        case SYSCALL_READ:
        {
            int fd = (int) p1;
            if(fd == 0)
                return ENOSYS;
            else if(fd == 1 || fd == 2)
                return EINVAL;
            return file_read(fd, (void*) p2, p3);
        }
        case SYSCALL_WRITE:
        {
            int fd = (int) p1;
            if(fd == 0)
                return EINVAL;
            else if(fd == 1 || fd == 2)
            {
                char *c = (char*) p2;
                int i = 0;
                for(i=0;i< (int)p3;i++)
                {
                    console_putc(c[i]);
                }
                return (int) p3;
            }
            return ENOSYS;
        }
        case SYSCALL_TEST:
        {
            kprintf("Syscall test! %d %d %d\n",
                p1,p2,p3
            );
            
            return (int)(p1+p2+p3);
        }
        case SYSCALL_HALT:
        {
            halt();
            return SUCCESS;
        }  

        default:
            return ENOSYS;
    }
}

