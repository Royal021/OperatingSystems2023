#include 'sd.h'

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
}

void delay_millisec(unsigned num){
    unsigned now = get_uptime();
    unsigned deadline = now+num;
    while( get_uptime() < deadline ){
        halt();
    }
}