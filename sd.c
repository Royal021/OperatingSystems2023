#include "sd.h"

static void sendCommand( u32 code, u32 arg, u32* returnValue);
static void sendAppCommand( u32 code, u32 arg, u32 cardAddress, u32* response);
static int setClock(u32 frequency);
static unsigned getHostControllerVersion();
static void waitForStatusBitsToBeZero(u32 mask);
static void waitForInterrupt(u32 mask);

void sd_init()
{
    *CONTROL0 = 0;
    *CONTROL1 = 0x01000000;
    int repetitions=100;
    while(1)
    {
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

    

//enable interrupts
*INTERRUPT_ENABLE = INTERRUPT_FLAGS_ALL_EVENTS;

//don't mask any interrupts
*INTERRUPT_MASK = INTERRUPT_FLAGS_ALL_EVENTS;

    sendCommand(COMMAND_GO_IDLE,0,NULL);
    sendCommand( COMMAND_SEND_INTERFACE_CONDITION, 0x1aa, NULL );
    delay_millisec(10);
    u32 operatingCondition;
    repetitions = 100;
    while(1)
    {
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
    for(int i=0;i<2;++i)
    {
        if( *STATUS & STATUS_READ_AVAILABLE ){
            (void)*DATA;    //read and discard
        } else {
            delay_millisec(1);
        }
    }
}

void delay_millisec(unsigned num)
{
    unsigned now = get_uptime();
    unsigned deadline = now+num;
    while( get_uptime() < deadline ){
        halt();
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
    }

    //write magic value
    *CONTROL1 = tmp;

    *CONTROL1 |= (1<<2);
    delay_millisec(10);

    repetitions = 100;
    while(1)
    {
        if( *CONTROL1 & (1<<1) )
            break;
        delay_millisec(10);
        --repetitions;
        if(repetitions==0)
            panic("Unstable clock");
    }

    return SUCCESS;     //0==success
}

static unsigned getHostControllerVersion()
{
    return ((*HOST_INFO)>>16)&0xff;
}

static void waitForInterrupt(u32 mask)
{
    int repetitions=100;
    while(1)
    {
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