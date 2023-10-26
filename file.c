#include "file.h"



struct File
{
    int in_use;
    int flags;
    unsigned offset;
    unsigned size;
    u32 firstCluster;
    u16 high;
    u16 low;
};




#define MAX_FILES 16        //real OS's use something
                            //like 1000 or so...
struct File fileTable[MAX_FILES];
static char clusterBuffer[CLUSTER_SIZE];




//Todo check not too long

int file_open(const char* fname, int flags)
{

   
    if(fname[0]=='\0')
    {
        
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
    
    err = read_cluster(2,clusterBuffer);
    if(err)
    {   
        
        goto cleanup;
    }

    struct DirEntry* D = (struct DirEntry*) clusterBuffer;
    
    
     
    int matchIndex = scanForMatchingFilename(fname, D);

    if(matchIndex<0)
    {
        
        err = matchIndex;
        goto cleanup;
    }

    
   fileTable[i].offset = 0;
    fileTable[i].firstCluster = (u16)(D[matchIndex].clusterHigh <<16) + D[matchIndex].clusterLow;
    //fileTable[i].offset +=fileTable[i].firstCluster;
    fileTable[i].size = D[matchIndex].size;
    
    fileTable[i].flags = flags; //flags is a parameter
    return i; //return file descriptor

    cleanup:
        fileTable[i].in_use = 0;
        //do anyother cleanun
        return err;

    
}

int scanForMatchingFilename(const char* fname, struct DirEntry ents[])
{

  
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
            
                return -1;
            }
            if(j>=3)
            {
                if(fname[i+j]!='\0')
                {
                    
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
        
        ext[j] = '\0';
        if (ext[0] == '\0'|| ext[1] =='\0' || ext[2] == '\0')
        {
            return -1;
        }
    }
    else
    {
        
        return -1;
    }
    if(kmemcmp(base, ents[s].base,8) !=0)
    {
        continue;
    }
    if(kmemcmp(ext,ents[s].ext,3) !=0)
    {
        continue;
    }
        return s;
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


int file_read(  int fd, void* buf,  unsigned capacity )
{
    
    if(fd < 0)
        return -1;  // put error code
    if(fileTable[fd].in_use<0)
        return EINVAL;  //error code
    if(capacity == 0)
        return 0;

    
    if(fileTable[fd].offset >= fileTable[fd].size)
    {
        
        return 0;
    } 

    int err = read_cluster( fileTable[fd].firstCluster,clusterBuffer);
    if(err != SUCCESS)
        return err;
    
    struct VBR* vbr = getVBR();

    



    unsigned clustersToSkip = fileTable[fd].offset/CLUSTER_SIZE;
    unsigned offsetInBuffer = fileTable[fd].offset % CLUSTER_SIZE;   //bytes to skip to skip
    //unsigned bytesToSkip = offsetInBuffer;
    // f is first clusters
    // 
    unsigned c = fileTable[fd].firstCluster;
   
    for(int i = 0; i<clustersToSkip; i++)
    {
        c = fat[c];
    }
    kprintf("cluster is %u" , c);
    disc_read_sectors(
        clusterNumberToSectorNumber(c),  
        vbr->sectors_per_cluster,
        clusterBuffer
    );
    unsigned remaingingBytesInCB = CLUSTER_SIZE-offsetInBuffer;
    unsigned numToCopy = Min32(remaingingBytesInCB, capacity);
   
    unsigned bytesLeftInFile = fileTable[fd].size - fileTable[fd].offset;
    numToCopy = Min32(numToCopy, bytesLeftInFile); 
    kmemcpy(buf, clusterBuffer+offsetInBuffer, numToCopy);
    fileTable[fd].offset += numToCopy;
    
    
        
//firstsector+reserve sector, count sectors per fat, static u32 fat)
//firstsector+reserve sector, count sectors per fat, static u32 fat)
    return (int) numToCopy;



}


int file_seek(int fd, int delta, int whence)
{
    if(whence<0||whence>2)
    {
        kprintf("11");
        return EINVAL;
    }
    if(fd < 0){
        kprintf("9");
        return EINVAL;
    }  
    
    if(fileTable[fd].in_use<0)
    {
    kprintf("1");
        return EINVAL;  
    }
   /*  if(fileTable[fd].offset >= fileTable[fd].size)
    {
        kprintf("2");
        return EINVAL;
    } */


    //seek_set
    if(whence == 0)
    {
        if(delta<0)
            return EINVAL;
        fileTable[fd].offset = (unsigned) delta;
    }   
    //seek_cur
    else if(whence ==1)
    {
        if (delta<0)
        {
            unsigned tmp = fileTable[fd].offset+ (unsigned) delta;
            if(tmp>fileTable[fd].offset){
                kprintf("4");
                return EINVAL;
            }
            fileTable[fd].offset=tmp;
        } else{
            //overflow case
              unsigned tmp = fileTable[fd].offset+ (unsigned) delta;
            if(tmp<fileTable[fd].offset){
                //overflow
                return -2;
            }
            fileTable[fd].offset=tmp;
        }
    }
    else if(whence== 2)
    {
        if (delta<0)
        {
            unsigned tmp = fileTable[fd].size+(unsigned) delta;
            if(tmp>fileTable[fd].size){
                kprintf("6");
                return EINVAL;
            }
            fileTable[fd].offset=tmp;
        } else{
            //overflow case
              unsigned tmp = fileTable[fd].size+ (unsigned) delta;
            if(tmp<fileTable[fd].size){
                //overflow
                kprintf("7");
                return EINVAL;
            }
            fileTable[fd].offset=tmp;
        }
    }
    return SUCCESS;
}



int file_tell(int fd, unsigned* offset)
{
    //store file offset to offset pointer
    if(offset==NULL)
    {
        return EINVAL;
    }
    if(fd<0)
        return -1;
    
    else
    {
        *offset = fileTable[fd].offset;
        
        return 0;
    }
    
}

int file_write( int fd,             //file to write to
                const void* buf,    //buffer for data
                unsigned count      //capacity of buf
){
    //no such system call
    return ENOSYS;
}


//offset/4096 = cluster toskip