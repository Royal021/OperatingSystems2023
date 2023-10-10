#include "file.h"
struct File
{
    int in_use;
    int flags;
};

#define MAX_FILES 16        //real OS's use something
                            //like 1000 or so...
struct File fileTable[MAX_FILES];

//Todo check not too long

int file_open(const char* fname, int flags)
{
    if(fname[0]==NULL)
    {
        return;
    }
    int i;
    
    for(i = 0; i<MAX_FILES; i++)
    {
        if(fileTable[i].in_use == 0)
        {
            fileTable[i].in_use = 1;
            break;
        }
    }
    if(i == MAX_FILES)
    {
        return EMFILE;
    }
    int err;
    static char clusterBuffer[4096];  // static so not on stack, goes onto the heap
    err = read_cluster(2,clusterBuffer);
    if(err)
    {
        goto cleanup;
    }

    struct Dirntry* D = (struct DirEntry*) clusterBuffer;
    int matchIndex = scanForMatchingFilename(fname, D);
    if(matchIndex<0)
    {
        err = matchIndex;
        goto cleanup;
    }

    fileTable[i].flags = flags; //flags is a parameter

    /* if(D[matchIndex].attributes& ATTRIB_READONLY){
        if(flags==O_WRONLY || flags == O_RDWR)
        {
            err = EPERM;
            goto cleanup;
        }
    }  */

    return i; //return file descriptor

    cleanup:
        fileTable[i].in_use = 0;
        //do anyother cleanun
        return err;

    
}

//InputL filename to scan for
//input list of dir entries to look at
//out put index of matching entry
// else error code(negative)
int scanForMatchingFilename(char* fname, struct DirEntry ents[])
{
    for(int i = 0; i<256; i++)
    {
        if(ents+i ==  fname)
        {
            return i;
        } 
    }
    return -1;
}

int file_close(int fd)
{
    if(fd<0 ||fd>=MAX_FILES)
        return EINVAL;

    if(fileTable[fd].in_use == 0)
        return EINVAL;

    fileTable[fd].in_use = 0;
    return SUCCESS;
}