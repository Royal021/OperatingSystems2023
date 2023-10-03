#include "fs.h"
#include "errno.h"

#pragma pack(push,1)
struct GUID {
    u32 time_low;
    u16 time_mid;
    u16 time_high_version;
    u16 clock;
    u8 node[6];
};
struct GPTEntry{
    struct GUID type;
    struct GUID guid;
    u32 firstSector;
    u32 firstSector64;
    u32 lastSector;
    u32 lastSector64;
    u32 attributes;
    u32 attributes64;
    u16 name[36];
};
#pragma pack(pop)

#pragma pack(push,1)
struct VBR{
    char jmp[3];
    char oem[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 reserved_sectors;
    u8 num_fats;
    u16 UNUSED_num_root_dir_entries;
    u16 UNUSED_num_sectors_small;
    u8 id ;
    u16 UNUSED_sectors_per_fat_12_16;
    u16 sectors_per_track;
    u16 num_heads;
    u32 first_sector;
    u32 num_sectors;
    u32 sectors_per_fat;
    u16 flags;
    u16 version;
    u32 root_cluster;
    u16 fsinfo_sector;
    u16 backup_boot_sector;
    char reservedField[12];
    u8 drive_number;
    u8 flags2;
    u8 signature;
    u32 serial_number;
    char label[11];
    char identifier[8];
    char code[420];
    u16 checksum;
};
#pragma pack(pop)

#pragma pack(push,1)
struct DirEntry {
    char base[8];
    char ext[3];
    unsigned char attributes;
    unsigned char reserved;
    unsigned char creationTimeCentiseconds;
    unsigned short creationTime;
    unsigned short creationDate;
    unsigned short lastAccessDate;
    unsigned short clusterHigh;
    unsigned short lastModifiedTime;
    unsigned short lastModifiedDate;
    unsigned short clusterLow;
    unsigned int size;
};
#pragma pack(pop)

#pragma pack(push,1)
struct LFNEntry {
    unsigned char sequenceNumber;
    char name0[10];             //5 characters
    char attribute;             //always 15
    char zero;                  //always zero
    char checksum;
    char name1[12];             //6 characters
    unsigned short alsozero;    //always zero
    char name2[4];              //2 characters
};
#pragma pack(pop)

static struct VBR vbr;
static unsigned first_sector;
static char sectorbuffer[512];
//char buff[4096];
char buff[4096];
char buff2[4096];

int disk_init()
{
    int rv = sd_read_sector(2, sectorbuffer);
    if(rv<0)
        return rv;
    struct GPTEntry* G = (struct GPTEntry *)sectorbuffer;
    first_sector = G[0].firstSector;  //G[0]. , (*G)., G->
    rv = sd_read_sector( first_sector, &vbr);
    if(rv<0)
        return rv;

    for(unsigned int p = 0; p<8;p++)
    {
    rv = sd_read_sector( clusterNumberToSectorNumber(2)+p, buff);
    struct DirEntry * d = (struct DirEntry*) buff;
    if(rv<0)
        return rv;
    for(int i = 0; i<4096; i++)
    {
        
        if(d[i].base[0] == 0x00)
        {
            //kprintf("\n");
            break;
        }
        if(d[i].attributes ==15 || d[i].base[0] == 0xe5)
        {
            continue;
        }
        else
        {
            int k = 0;
            while(d[i].base[k]!=' ' && k<8)
            {
                kprintf("%c", d[i].base[k]);
                k++;
            }
        kprintf(".%s\n", d[i].ext);
        }        
    }
    }
    
    return SUCCESS;
}

unsigned clusterNumberToSectorNumber( unsigned clnum )
{
    unsigned curSector = first_sector;
    curSector += vbr.sectors_per_fat *vbr.num_fats + vbr.reserved_sectors + vbr.sectors_per_cluster*(clnum-2);
    return curSector;
}