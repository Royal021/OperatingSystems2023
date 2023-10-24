#include "fs.h"

static struct VBR vbr;

static unsigned first_sector;
static char sectorbuffer[512];
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
                //kprintf("%c", d[i].base[k]);
                k++;
            }
        //kprintf(".%s\n", d[i].ext);
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


int read_cluster(unsigned clnum, void* buffer)
{
    char *p = (char*) buffer;
    unsigned secnum = clusterNumberToSectorNumber(clnum);
    for(unsigned i = 0; i<8; i++)
    {
        int rv = sd_read_sector(secnum+i, p+i*512);
        if(rv!=SUCCESS)
            return rv;
    }
    return SUCCESS;
}

struct VBR* getVBR()
{
    return &vbr;
};

