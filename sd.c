#include "sd.h"
#include "utils.h"
#include "timer.h"
#include "kprintf.h"
#include "errno.h"


#define DO_LONG_NAMES 0
#define DO_CTIME 0



#define EMMC_BASE  (PERIPHERAL_BASE+0x00300000)
#define CONTROL0  ( (volatile u32*)(EMMC_BASE+0x28) )
#define CONTROL1   ( (volatile u32*)(EMMC_BASE+0x2c) )

#define COMMAND                                 ( (volatile u32*)(EMMC_BASE+0x0c) )
#define INDEX_FROM_COMMAND(x)                   ( (x>>24) & 0x3f )
#define COMMAND_INDEX(x)                        ( x<<24 )
#define COMMAND_GO_IDLE                         ( COMMAND_INDEX(0) )
#define COMMAND_HAS_RESPONSE_6_BYTES            ( 2<<16 )
#define COMMAND_SEND_INTERFACE_CONDITION        ( COMMAND_INDEX(8) | COMMAND_HAS_RESPONSE_6_BYTES )
#define APP_COMMAND_SEND_OPERATING_CONDITION    ( COMMAND_INDEX(41) | COMMAND_HAS_RESPONSE_6_BYTES                                                      )
#define COMMAND_HAS_RESPONSE_17_BYTES           ( 1<<16 )
#define COMMAND_SEND_CARD_ID                    ( COMMAND_INDEX(2)  | COMMAND_HAS_RESPONSE_17_BYTES                                                     )
#define COMMAND_SEND_RELATIVE_ADDRESS           ( COMMAND_INDEX(3)  | COMMAND_HAS_RESPONSE_6_BYTES                                                      )
#define COMMAND_HAS_RESPONSE_6_BYTES_WITH_BUSY  ( 3<<16 )
#define COMMAND_CARD_SELECT                     ( COMMAND_INDEX(7)  | COMMAND_HAS_RESPONSE_6_BYTES_WITH_BUSY                                            )
#define DATA                                    ( (volatile u32*)(EMMC_BASE+0x20) )
#define BLOCK_SIZE_AND_COUNT                    ( (volatile u32*)(EMMC_BASE+0x04) )
#define STATUS                                  ( (volatile u32*)(EMMC_BASE+0x24) )
#define STATUS_READ_AVAILABLE                   (1<<9)
#define STATUS_DATA_INHIBIT                     (1<<1)
#define MAKE_BLOCK_SIZE_AND_COUNT( size,count ) ( (count<<16) | size )
#define COMMAND_INVOLVES_DATA                   ( 1<<21 )
#define COMMAND_READS_FROM_CARD                 ( 1<<4  )
#define APP_COMMAND_SEND_SCR  (  COMMAND_INDEX(51) |                   \
                                 COMMAND_INVOLVES_DATA |               \
                                 COMMAND_HAS_RESPONSE_6_BYTES |        \
                                 COMMAND_READS_FROM_CARD )
#define STATUS_CMD_INHIBIT                      (1)
#define INTERRUPT_FLAGS                         ( (volatile u32*)(EMMC_BASE+0x30) )
#define ARG1                                    ( (volatile u32*)(EMMC_BASE+0x08) )
#define INTERRUPT_FLAGS_COMMAND_DONE            (1)
#define RESPONSE0                               ( (volatile u32*)(EMMC_BASE+0x10) )
#define RESPONSE1                               ( (volatile u32*)(EMMC_BASE+0x14) )
#define RESPONSE2                               ( (volatile u32*)(EMMC_BASE+0x18) )
#define RESPONSE3                               ( (volatile u32*)(EMMC_BASE+0x1c) )
#define HOST_INFO                   ( (volatile u32*)(EMMC_BASE+0xfc) )
//If a bit in here is 1, the corresponding interrupt is enabled
#define INTERRUPT_MASK                      ( (volatile u32*)(EMMC_BASE+0x34) )
#define INTERRUPT_ENABLE              ( (volatile u32*)(EMMC_BASE+0x38) )
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

//all the non-reserved bits in the interrupt register
#define INTERRUPT_FLAGS_ALL_EVENTS          (0x017ff137)
//all the error bits in the interrupt register
#define INTERRUPT_FLAGS_ALL_ERROR_BITS      (0x017e8000)


#define COMMAND_IS_MULTIBLOCK                   ( 1<<5  )
#define COMMAND_WRITES_TO_CARD                  ( 0     )

#define COMMAND_READ_SINGLE                     ( COMMAND_INDEX(17) | COMMAND_INVOLVES_DATA | COMMAND_HAS_RESPONSE_6_BYTES | COMMAND_READS_FROM_CARD    )
#define COMMAND_WRITE_SINGLE                    ( COMMAND_INDEX(24) | COMMAND_INVOLVES_DATA | COMMAND_HAS_RESPONSE_6_BYTES | COMMAND_WRITES_TO_CARD     )



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

struct VBR vbr;

#define MAX_DISK_SIZE  (128*1024*1024)
#define MAX_CLUSTERS (MAX_DISK_SIZE/4096)

u32 fat[MAX_CLUSTERS];


static unsigned getHostControllerVersion()
{
    return ((*HOST_INFO)>>16)&0xff;
}


void delay_millisec(unsigned num){
    unsigned now = get_uptime();
    unsigned deadline = now+num;
    while( get_uptime() < deadline ){
        halt();
    }
}


static void waitForStatusBitsToBeZero(u32 mask)
{
    int repetitions=100;
    while(1){
        u32 tmp = *INTERRUPT_FLAGS;
        if( 0 != (tmp & INTERRUPT_FLAGS_ALL_ERROR_BITS) ){
            panic("I/O error waiting for status bits");
        }

        tmp = *STATUS;

        if( 0 == (tmp & mask) ){
            return;
        }
        --repetitions;
        if(repetitions==0)
            panic("Timeout waiting for status bits");
        delay_millisec(1);
    }
}

static void waitForInterrupt(u32 mask)
{
    //wait for any of the interrupt bits in mask to become 1
    //Also stop if any error bits get set
    int repetitions=100;
    while(1){
        u32 tmp = *INTERRUPT_FLAGS;
        if( tmp & INTERRUPT_FLAGS_ALL_ERROR_BITS ){
            *INTERRUPT_FLAGS = INTERRUPT_FLAGS_ALL_ERROR_BITS;
            panic("I/O error");
        }
        if( tmp & mask ){
            //clear the bits
            *INTERRUPT_FLAGS = mask;
            return;
        }
        --repetitions;
        if(repetitions==0)
            panic("Timeout waiting for interrupt");
        delay_millisec(1);
    }

}


static void sendCommand( u32 code, u32 arg, u32* returnValue)
{
    waitForStatusBitsToBeZero(STATUS_CMD_INHIBIT);

    *INTERRUPT_FLAGS = *INTERRUPT_FLAGS;
    *ARG1 = arg;
    *COMMAND = code;
    u32 op = INDEX_FROM_COMMAND(code);
    unsigned delay;
    if( op == 1 )       //SEND_OP_CND
        delay = 1000;
    else if( op == 8 || op == 55 ) //8=SEND_OP_CND, 55=APP_CMD
        delay = 100;
    else
        delay = 0;

    if( delay > 0 )
        delay_millisec(delay);

    waitForInterrupt( INTERRUPT_FLAGS_COMMAND_DONE );
    if( returnValue != NULL ){
        if( code & COMMAND_HAS_RESPONSE_6_BYTES ){
            *returnValue = *RESPONSE0;
        } else if( code & COMMAND_HAS_RESPONSE_17_BYTES ){
            returnValue[0] = *RESPONSE0;
            returnValue[1] = *RESPONSE1;
            returnValue[2] = *RESPONSE2;
            returnValue[3] = *RESPONSE3;
        } else {
            panic("This command does not issue a response\n");
        }
    }

}


static void sendAppCommand( u32 code, u32 arg, u32 cardAddress,
                           u32* response)
{
    u32 tmp = COMMAND_INDEX(55);
    if( cardAddress != 0 ){
        tmp |= COMMAND_HAS_RESPONSE_6_BYTES;
    }
    sendCommand( tmp, cardAddress, NULL);
    sendCommand( code, arg, response);
}

static int setClock(u32 frequency)
{
    int repetitions = 100;
    while( 1 ){
        u32 tmp = *STATUS;
        if( (tmp & STATUS_CMD_INHIBIT) == 0 &&
            (tmp & STATUS_DATA_INHIBIT) == 0 ){
                break;
        }
        --repetitions;
        if(repetitions==0){
            panic("SD timeout setting clock");
        }
        delay_millisec(1);
    }


    *CONTROL1 &= (u32)(~(1<<2));
    delay_millisec(10);
    u32 tmp = *CONTROL1;
    tmp &= (u32)(~0xff00);  //mask bits 8-15

    //set magic values
    if( frequency == 25000000 ){
        tmp |= (0x02)<<8;
    } else if( frequency == 400000 ){
        if( getHostControllerVersion() < 2 )
            tmp |= (0x40) << 8;
        else
            tmp |= (0x68) << 8;
    } else {
        panic("Bad clock\n");
    }

    //write magic value
    *CONTROL1 = tmp;
    *CONTROL1 |= (1<<2);
    delay_millisec(10);

    repetitions = 100;
    while(1){
        if( *CONTROL1 & (1<<1) )
            break;
        delay_millisec(10);
        --repetitions;
        if(repetitions==0)
            panic("Unstable clock");
    }

    return 0;       //success
}


int printLfnPiece( char* c, int sz ){
    int i;
    for(i=0;i<sz;i+=2){
        if( c[i] == 0 )
            return 1;
        kprintf("%c",c[i]);
    }
    return 0;
}


unsigned clusterNumberToSectorNumber( unsigned clnum ){
    return  vbr.first_sector +
            vbr.reserved_sectors +
            vbr.num_fats*vbr.sectors_per_fat +
            (clnum-2)*vbr.sectors_per_cluster;
}


void sd_init()
{
    *CONTROL0 = 0;
    *CONTROL1 = 0x01000000;

    int repetitions=100;
    while(1){
        delay_millisec(10);
        if( 0 == (*CONTROL1 & 0x01000000)  ){
            break;
        }
        repetitions--;
        if( repetitions == 0 ){
            panic("Cannot reset SD host");
        }
    }
    *CONTROL1 = (0xe << 16) | 1;
    delay_millisec(10);

    setClock(400000);

    *INTERRUPT_ENABLE = INTERRUPT_FLAGS_ALL_EVENTS;
    *INTERRUPT_MASK = INTERRUPT_FLAGS_ALL_EVENTS;

    sendCommand( COMMAND_GO_IDLE, 0, NULL );
    sendCommand( COMMAND_SEND_INTERFACE_CONDITION, 0x1aa, NULL );
    delay_millisec(10);
    u32 operatingCondition;
    repetitions = 100;
    while(1){
        delay_millisec(10);

        sendAppCommand(
            APP_COMMAND_SEND_OPERATING_CONDITION,
            0x51ff8000,
            0,
            &operatingCondition
        );

        //if high bit is set, command has completed
        if( operatingCondition & 0x80000000 ){
            if( 0 == (operatingCondition & 0xff8000) ){
                panic("Voltage bits are invalid!");
            }
            break;
        }
        --repetitions;
        if(repetitions == 0 )
            panic("Cannot get operating condition");
    }

    unsigned cardID[4];
    sendCommand(COMMAND_SEND_CARD_ID,0,cardID);
    u32 cardAddress;
    sendCommand(COMMAND_SEND_RELATIVE_ADDRESS,0, &cardAddress );
    setClock(25000000);
    sendCommand(COMMAND_CARD_SELECT, cardAddress, NULL);
    waitForStatusBitsToBeZero(STATUS_DATA_INHIBIT);
    *BLOCK_SIZE_AND_COUNT =  MAKE_BLOCK_SIZE_AND_COUNT(8, 1);
    sendAppCommand(APP_COMMAND_SEND_SCR, 0, cardAddress, NULL);
    waitForInterrupt(INTERRUPT_FLAGS_READ_READY);
    //read 8 bytes: two u32's
    for(int i=0;i<2;++i){
        if( *STATUS & STATUS_READ_AVAILABLE ){
            (void)*DATA;    //read and discard
        } else {
            delay_millisec(1);
        }
    }

    static char sectorData[4096];
    sd_read_sector(2,sectorData);
    struct GPTEntry* entry = (struct GPTEntry*) sectorData;
    sd_read_sector( entry->firstSector, &vbr);
    sd_read_sectors( clusterNumberToSectorNumber(2),
        vbr.sectors_per_cluster, sectorData);
    if(vbr.sectors_per_fat*512/4 >= MAX_CLUSTERS ){
        kprintf("Disk is too big! (Must be less than %d bytes)\n",
            MAX_DISK_SIZE);
        panic("Halting.");
    }

    if( SUCCESS != sd_read_sectors( vbr.first_sector + vbr.reserved_sectors,
        vbr.sectors_per_fat, fat ) ){
            panic("Error reading FAT");
    }

}


int sd_read_sector(unsigned sector, void* buffer)
{
    waitForStatusBitsToBeZero(STATUS_DATA_INHIBIT);
    *BLOCK_SIZE_AND_COUNT = MAKE_BLOCK_SIZE_AND_COUNT(512,1);
    sendCommand( COMMAND_READ_SINGLE, sector*512, NULL );
    waitForInterrupt(INTERRUPT_FLAGS_READ_READY);
    u32* p = (u32*) buffer;
    for(int i=0;i<128;++i){
        *p = *DATA;
        p++;
    }
    return SUCCESS;
}


int sd_write_sector(unsigned sector, const void* buffer)
{
    waitForStatusBitsToBeZero(STATUS_DATA_INHIBIT);
    *BLOCK_SIZE_AND_COUNT = MAKE_BLOCK_SIZE_AND_COUNT(512,1);
    sendCommand( COMMAND_WRITE_SINGLE, sector*512, NULL );
    waitForInterrupt(INTERRUPT_FLAGS_WRITE_READY);
    u32* p = (u32*) buffer;
    for(int i=0;i<128;++i){
        *DATA = *p;
        p++;
    }
    return SUCCESS;
}

int sd_read_sectors(unsigned sector, unsigned count, void* buffer)
{
    char* p = (char*) buffer;
    for(unsigned i=0;i<count;++i){
        int rv = sd_read_sector(sector+i, p+i*512 );
        if(rv != SUCCESS )
            return rv;
    }
    return SUCCESS;
}
