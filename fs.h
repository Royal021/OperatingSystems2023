#include "sd.h"
#include "utils.h"
typedef unsigned long long u64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

int disk_init();
unsigned clusterNumberToSectorNumber( unsigned clnum );