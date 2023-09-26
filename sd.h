#include "timer.h"
#include "utils.h"

#define EMMC_BASE  (PERIPHERAL_BASE+0x00300000)
#define CONTROL0  ( (volatile u32*)(EMMC_BASE+0x28) )
#define CONTROL1   ( (volatile u32*)(EMMC_BASE+0x2c) )