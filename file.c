#include "file.h"
#include "errno.h"
#include "utils.h"
#include "sd.h"
#include "kprintf.h"

struct File{
    int in_use;
    int flags;
    unsigned offset;
    unsigned size;
    unsigned start;
};


#define MAX_FILES 16
struct File fileTable[MAX_FILES];

int file_open(const char* filename, int flags)
{
    int fd;
    for(fd=3;fd<MAX_FILES;++fd){
        if( !fileTable[fd].in_use )
            break;
    }
    if( fd == MAX_FILES ){
        return EMFILE;
    }

    static char clusterBuffer[4096];
    sd_read_sectors(clusterNumberToSectorNumber(2), 8, clusterBuffer);

    //get base from input
    char base[8];
    char ext[3];
    int i;

    for(i=0;i<8;++i){
        base[i]=' ';
    }
    for(i=0;i<3;++i){
        ext[i]=' ';
    }

    for(i=0; ;++i){
        char c = filename[i];
        if( c == '.' )
            break;
        if( c == 0 )
            break;

        if( i==8 ){
            //base is too long
            return ENOENT;
        }

        //reject spaces since the rules are odd for them
        if( c == ' ' ){
            return ENOENT;
        }
        base[i] = toupper(filename[i]);
    }

    if( filename[i] == 0 ){
        //ext is empty; leave as-is
    } else {
        if( filename[i] != '.' )
            panic("Internal error");
        i++;
        int j=0;
        for( ; ; ++i,j++){
            char c = filename[i];
            if( c == 0 )
                break;

            if( j == 3 || c == '.' || c == ' '){
                //no match
                return ENOENT;
            }
            ext[j]=toupper(filename[i]);
        }
    }

    struct DirEntry* de = (struct DirEntry*) clusterBuffer;
    //NOTE: This only works if root directory takes < 1 cluster

    int ok=0;

    for( ; de->base[0] != 0 ; de++ ){
        if( de->attributes == 15 )
            continue;       //LFN entry
        if( 0 == kmemcmp(base,de->base,8) &&
            0 == kmemcmp(ext,de->ext,3) ){
                ok=1;
                break;
        }
    }

    if( ok ){
        fileTable[fd].in_use = 1;
        fileTable[fd].flags = flags;    //not used...
        fileTable[fd].offset = 0;
        fileTable[fd].size = de->size;
        fileTable[fd].start = (unsigned)((de->clusterHigh << 16) | (de->clusterLow));
        return fd;
    } else {
        return ENOENT;
    }
}

int file_close(int fd)
{
    if( fd >= 0 && fd < MAX_FILES && fileTable[fd].in_use ){
        fileTable[fd].in_use=0;
        return SUCCESS;
    } else {
        return EINVAL;
    }
}

int file_read(int fd, void* buffer, unsigned count)
{
    if( fd < 0 || fd >= MAX_FILES || fileTable[fd].in_use == 0 )
        return EINVAL;
    if( count == 0 )
        return 0;

    //avoid dealing with signed/unsigned overflow
    if( count > 0x7fffffff )
        count = 0x7fffffff;


    if( fileTable[fd].offset >= fileTable[fd].size )
        return 0;

    static char clusterData[4096];

    unsigned clustersToSkip = fileTable[fd].offset / 4096;
    unsigned clusterNumber = fileTable[fd].start;
    while(clustersToSkip != 0 ){
        clusterNumber = fat[clusterNumber];
        clustersToSkip--;
    }

    unsigned sector = clusterNumberToSectorNumber(clusterNumber);
    sd_read_sectors(sector,8,clusterData);
    unsigned offsetInBuffer = fileTable[fd].offset % 4096;
    unsigned bytesLeftInFile = fileTable[fd].size - fileTable[fd].offset;
    unsigned bytesLeftInCluster = 4096 - offsetInBuffer;
    unsigned numBytesToCopy = min( count, min( bytesLeftInFile, bytesLeftInCluster)  );
    kmemcpy(buffer,clusterData+offsetInBuffer, numBytesToCopy );
    fileTable[fd].offset += numBytesToCopy;
    //numBytesToCopy will never be > 2GB, so no overflow
    return (int)numBytesToCopy;
}

int file_write(int fd, void* buffer, unsigned count)
{
    return ENOSYS;
}

int file_seek(int fd, int delta, int whence)
{
    if( fd < 0 || fd >= MAX_FILES || fileTable[fd].in_use == 0 )
        return EINVAL;
    if( whence == SEEK_SET ){
        if(delta < 0 )
            return EINVAL;
        fileTable[fd].offset = (unsigned) delta;
        return SUCCESS;
    }
    if( whence == SEEK_CUR ){
        unsigned newOffset = fileTable[fd].offset + (unsigned)delta;
        if( delta < 0 ){
            if( newOffset > fileTable[fd].offset )
                return EINVAL;
            fileTable[fd].offset = newOffset;
        } else {
            if( newOffset < fileTable[fd].offset )
                return EINVAL;
            fileTable[fd].offset = newOffset;
        }
        return SUCCESS;
    }
    if( whence == SEEK_END ){
        unsigned newOffset = fileTable[fd].size + (unsigned)delta;
        if( delta < 0 ){
            if( newOffset >= fileTable[fd].size )
                return EINVAL;
            else
                fileTable[fd].offset = newOffset;
        } else {
            if( newOffset < fileTable[fd].size )
                return EINVAL;
            else
                fileTable[fd].offset = newOffset;
        }
        return SUCCESS;
    }

    return EINVAL;

}

int file_tell(int fd, unsigned* offset)
{
    if( fd < 0 || fd >= MAX_FILES || fileTable[fd].in_use == 0 || offset == 0)
        return EINVAL;
    *offset = fileTable[fd].offset;
    return SUCCESS;
}

int file_read_fully(int fd, void* buf, unsigned count)
{
    if(count > 0x7fffffff){
        panic("Bad value");
    }
    int nr=0;
    char* p = (char*) buf;
    while(count > 0 ){
        int rv = file_read(fd,p,count);
        if(rv<0){
            return rv;
        }
        if(rv == 0 ){
            return nr;
        }
        p += rv;
        count -= (unsigned)rv;
        nr += rv;
    }
    return nr;
}
