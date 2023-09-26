#pragma once

#include "timer.h"
#include "utils.h"
#include "errno.h"

#define EMMC_BASE  (PERIPHERAL_BASE+0x00300000)
#define CONTROL0  ( (volatile u32*)(EMMC_BASE+0x28) )
#define CONTROL1   ( (volatile u32*)(EMMC_BASE+0x2c) )

//If a bit in here is 1, the corresponding interrupt is enabled
#define INTERRUPT_MASK    ( (volatile u32*)(EMMC_BASE+0x34) )
#define INTERRUPT_ENABLE  ( (volatile u32*)(EMMC_BASE+0x38) )

//all the non-reserved bits in the interrupt register
#define INTERRUPT_FLAGS_ALL_EVENTS          (0x017ff137)

#define COMMAND_INDEX(x)        ( x<<24 )
#define COMMAND_GO_IDLE         ( COMMAND_INDEX(0) )

#define COMMAND_HAS_RESPONSE_6_BYTES            ( 2<<16 )
#define COMMAND_SEND_INTERFACE_CONDITION    \
    ( COMMAND_INDEX(8) | COMMAND_HAS_RESPONSE_6_BYTES )

#define APP_COMMAND_SEND_OPERATING_CONDITION    \
    ( COMMAND_INDEX(41) | COMMAND_HAS_RESPONSE_6_BYTES )

#define COMMAND_HAS_RESPONSE_17_BYTES           ( 1<<16 )
#define COMMAND_SEND_CARD_ID                    ( COMMAND_INDEX(2) |\
                                        COMMAND_HAS_RESPONSE_17_BYTES)
#define COMMAND_SEND_RELATIVE_ADDRESS           ( COMMAND_INDEX(3) |\
                                        COMMAND_HAS_RESPONSE_6_BYTES)

#define COMMAND_HAS_RESPONSE_6_BYTES_WITH_BUSY  ( 3<<16 )
#define COMMAND_CARD_SELECT                     ( COMMAND_INDEX(7) |\
                                COMMAND_HAS_RESPONSE_6_BYTES_WITH_BUSY )


#define STATUS                      ( (volatile u32*)(EMMC_BASE+0x24) )
#define DATA                        ( (volatile u32*)(EMMC_BASE+0x20) )
#define BLOCK_SIZE_AND_COUNT        ( (volatile u32*)(EMMC_BASE+0x04) )
#define STATUS_READ_AVAILABLE       (1<<9)
#define STATUS_DATA_INHIBIT         (1<<1)
#define MAKE_BLOCK_SIZE_AND_COUNT( size,count )  ( (count<<16) | size )
#define COMMAND_INVOLVES_DATA                   ( 1<<21 )
#define COMMAND_READS_FROM_CARD                 ( 1<<4  )
#define APP_COMMAND_SEND_SCR               (\
             COMMAND_INDEX(51) |            \
             COMMAND_INVOLVES_DATA |        \
             COMMAND_HAS_RESPONSE_6_BYTES | \
             COMMAND_READS_FROM_CARD        )

#define STATUS_CMD_INHIBIT  (1)
#define INTERRUPT_FLAGS     ( (volatile u32*)(EMMC_BASE+0x30) )

#define ARG1     ( (volatile u32*)(EMMC_BASE+0x08) )
#define COMMAND  ( (volatile u32*)(EMMC_BASE+0x0c) )

#define INDEX_FROM_COMMAND(x)                   ( (x>>24) & 0x3f )

#define INTERRUPT_FLAGS_COMMAND_DONE        (1   )
#define RESPONSE0                   ( (volatile u32*)(EMMC_BASE+0x10) )
#define RESPONSE1                   ( (volatile u32*)(EMMC_BASE+0x14) )
#define RESPONSE2                   ( (volatile u32*)(EMMC_BASE+0x18) )
#define RESPONSE3                   ( (volatile u32*)(EMMC_BASE+0x1c) )

#define HOST_INFO                   ( (volatile u32*)(EMMC_BASE+0xfc) )

//interrupt flags; called INTERRUPT in documentation
//This tells which interrupt(s) are being raised.
#define INTERRUPT_FLAGS_COMMAND_ERROR       (1<<24)
#define INTERRUPT_FLAGS_DATA_END_ERROR      (1<<22)
#define INTERRUPT_FLAGS_DATA_CRC_ERROR      (1<<21)
#define INTERRUPT_FLAGS_DATA_TIMEOUT        (1<<20)
#define INTERRUPT_FLAGS_BAD_INDEX           (1<<19)
#define INTERRUPT_FLAGS_COMMAND_END_ERROR   (1<<18)
#define INTERRUPT_FLAGS_COMMAND_CRC_ERROR   (1<<17)
#define INTERRUPT_FLAGS_COMMAND_TIMEOUT     (1<<16)
#define INTERRUPT_FLAGS_GENERIC_ERROR       (1<<15)
//non-error events
#define INTERRUPT_FLAGS_DATA_TRANSFER_DONE  (1<<1)
#define INTERRUPT_FLAGS_BLOCK_GAP           (1<<2)
#define INTERRUPT_FLAGS_WRITE_READY         (1<<4)
#define INTERRUPT_FLAGS_READ_READY          (1<<5)
#define INTERRUPT_FLAGS_CARD_IRQ            (1<<8)
#define INTERRUPT_FLAGS_CLOCK_RETUNE        (1<<12)
#define INTERRUPT_FLAGS_BOOT_ACK            (1<<13)
#define INTERRUPT_FLAGS_BOOT_END            (1<<14)

//all the error bits in the interrupt register
#define INTERRUPT_FLAGS_ALL_ERROR_BITS      (0x017e8000)

void delay_millisec(unsigned num);
void sd_init();


int sd_write_sector(unsigned sector, const void* buffer);
int sd_read_sector(unsigned sector, void* buffer);