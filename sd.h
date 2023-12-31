#pragma once


#include "utils.h"

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
    unsigned char creationTimeSecondsTenths;
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
	char name0[10];
	char attributes;	//always 15
	char zero;		//always zero
	char checksum;
	char name1[12];
	unsigned short alsozero;	//always zero
	char name2[4];
};
#pragma pack(pop)

extern struct VBR vbr;
extern u32 fat[];

void sd_init();
int sd_read_sector(unsigned sector, void* buffer);
int sd_read_sectors(unsigned sector, unsigned count, void* buffer);
unsigned clusterNumberToSectorNumber( unsigned clnum );
