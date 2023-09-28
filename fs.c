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

static unsigned first_sector;
static char sectorbuffer[512];
int disk_init()
{
    int rv = sd_read_sector(2, sectorbuffer);
    if(rv<0)
        return rv;
    struct GPTEntry* G= (struct GPTEntry *)sectorbuffer;
    first_sector = G->first_sector;  //G[0]. , (*G).
    return SUCCESS;
}