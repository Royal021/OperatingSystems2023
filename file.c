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
    if(fname[0]=='\0')
    {
        kprintf("here");
        return EMFILE;
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

    struct DirEntry* D = (struct DirEntry*) clusterBuffer;
    
    //heres the problem
  
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
//write a function that takes a filename and a list of dir entries and converts the filename a 8 character base and 3 character extension
    //then compares the base and extension to the base and extension of each dir entry
int scanForMatchingFilename(const char* fname, struct DirEntry ents[]){

  
    char base[9];
    char ext[4];
    for(int s = 0; s<256; s++)
    {

    if (ents[s].base[0] == '\0')
    {
        continue;

    }
    if(ents[s].ext[0] == '\0')
    {
        continue;
    }
    int i;
    for(i = 0; i<8; i++)
    {
        if(fname[i]==' ')
            return -1;
        if(fname[i]=='.' || fname[i]=='\0')
            break;
        base[i] = toupper(fname[i]);
    }
    int k = i;
    if(i<8)
    {
        for(;k<8;k++)
        {
            base[k] = ' ';
        }
    }
    if(fname[i] != '.')
        {
            kprintf("period here");
            return -1;
        }
    base[k] = '\0';
    
    if(fname[i]=='.')
    {
        i++;
        int j = 0;
        int good = 1;
    
        while(good)
        {
            if(fname[i+j]==' ')
            {
            kprintf("ext too short here1");
                return -1;
            }
            if(j>=3)
            {
                if(fname[i+j]!='\0')
                {
                    kprintf("ext too long here2");
                    return -1;
                }
                good = 0;
                break;
            }
            if(fname[i+j]=='\0')
            {
                good = 0;
                break;
            }
            ext[j] = toupper(fname[i+j]);
            j++;
        }
        while(j<3)
        {
            ext[j]=' ';
            j++;
        }
        ext[j] = '\0';
        /* if(fname[i+j] != '\0')
        {
            kprintf("ext too long here");
            return -1;
        } */
        ext[j] = '\0';
        if (ext[0] == '\0'|| ext[1] =='\0' || ext[2] == '\0')
        {
            kprintf("ext too short here3");
            return -1;
        }
        
        
    }
    else
    {
        kprintf("theres not a period in file");
        return -1;
    }
    
   

 
    //kprintf("entbase: %s\n", ents[s].base);
    //kprintf("entext: %s\n", ents[s].ext);
    
    if(kmemcmp(base, ents[s].base,8) !=0)
    {
        continue;
    }
    if(kmemcmp(ext,ents[s].ext,3) !=0)
    {
        continue;
    }
        return i;
    }
    kprintf("base: %s\n", base);
    kprintf("ext: %s\n", ext);
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