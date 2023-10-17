
//change this to 1 to get lots of debugging messages
#define VERBOSE 1






#include "kprintf.h"

int file_open(const char* fname, int flags);
int file_read(int fd, void* buffer, unsigned count);
int file_seek(int fd, int offset, int whence);
int file_tell(int fd, unsigned* offset);


#if VERBOSE
#define vprintf kprintf
#else
static void vprintf(const char* fmt, ... ){
}
#endif

static const char article6[] = {
    "Article. VI.\n\n"
    "All Debts contracted and Engagements entered into, before the Adoption "
    "of this Constitution, shall be as valid against the United States under "
    "this Constitution, as under the Confederation.\n\n"
    "This Constitution, and the Laws of the United States which shall be made "
    "in Pursuance thereof; and all Treaties made, or which shall be made, "
    "under the Authority of the United States, shall be the supreme Law of the "
    "Land; and the Judges in every State shall be bound thereby, any Thing in the "
    "Constitution or Laws of any State to the Contrary notwithstanding.\n\n"
    "The Senators and Representatives before mentioned, and the Members of "
    "the several State Legislatures, and all executive and judicial Officers, "
    "both of the United States and of the several States, shall be bound by Oath or "
    "Affirmation, to support this Constitution; but no religious Test shall ever "
    "be required as a Qualification to any Office or public Trust under the United States." };

static const char article5[] = {
    "Article. V.\n\n"
    "The Congress, whenever two thirds of both Houses shall deem it necessary, "
    "shall propose Amendments to this Constitution, or, on the Application of the "
    "Legislatures of two thirds of the several States, shall call a Convention "
    "for proposing Amendments, which, in either Case, shall be valid to all Intents "
    "and Purposes, as Part of this Constitution, when ratified by the Legislatures "
    "of three fourths of the several States, or by Conventions in three fourths thereof, "
    "as the one or the other Mode of Ratification may be proposed by the Congress; "
    "Provided that no Amendment which may be made prior to the Year One thousand eight "
    "hundred and eight shall in any Manner affect the first and fourth Clauses "
    "in the Ninth Section of the first Article; and that no State, without its Consent, "
    "shall be deprived of its equal Suffrage in the Senate."
};

struct Seeks{
    int delta;
    int whence;
};

static struct Seeks seeks[] = {
    {3,0},
    {14,0},
    {4096,0},
    {159,0},
    {4095,0},
    {-2010,0},
    {26,0},
    {-1,0},
    {53,0},
    {5,0},
    {89,0},
    {5000,0},
    {20000,0},
    {79,0},
    {323,0},
    {0,0},
    {4097,0},
    {sizeof(article5),0},
    {52, 0},
    {sizeof(article5)-1,0},
    {sizeof(article5)-2,0},
    {3, 2},
    {14,2},
    {159,2},
    {20000,2},
    {5020,2},
    {1,2},
    {4096,2},
    {409,2},
    {4095,2},
    {20000,2},
    {100,2},
    {4097,2},
    {100,2},
    {5906,2},
    {0,2},
    {16,2},
    {(int)sizeof(article5),2},
    {20,2},
    {(int)sizeof(article5)+1,2},
    {50,2},
    {(int)sizeof(article5)-1,2},
    {-(int)(sizeof(article5)-4),2},
    {-5,2},
    {77,2},
    {(int)sizeof(article5)-2,2},
    {-(int)(sizeof(article5)-2),2},
    {-10,2},
    {-5,2},
    {26,0},
    {-1,2},
    {-10,1},
    {26,0},
    {50,1},
    {-13,1},
    {1000,1},
    {26,0},
    {-4000,1},
    {26,0},
    {4000,1},
    {26,0},
    {-4099,1},
    {26,0},
    {4099,1},
    {26,0},
    { 0, 1},
    {26,0},
    {-1, 1},
    { 20000, 1},
    {-20000, 1},
    {26,0},
    {-20,1},
    {20,1},
    {-4000,2},
    {0x7fffffff,0},     //2G
    {0x7fffffff,1},     //just short of 4G
    {0x7fffffff,1},     //overflow; should be invalid
    {-1,-1}
};


static void memdump(void* p, int count){
    const int charsPerLine=16;
    unsigned char* c = (unsigned char*) p;
    while( count > 0 ){
        int i;
        for(i=0; i<charsPerLine && i < count;++i){
            kprintf("%02x ",c[i]);
        }
        for( ; i<charsPerLine; ++i ){
            kprintf("   ");
        }
        kprintf(" | ");
        for(i=0; i<charsPerLine && i < count;++i){
            if( c[i] >= 32 && c[i] <= 126 )
                kprintf("%c",c[i]);
            else
                kprintf(".");
        }
        kprintf("\n");
        count-=charsPerLine;
        c += charsPerLine;
    }
}


//~ static void mystrcpy(char* dest, const char* src){
    //~ while(1){
        //~ *dest=*src;
        //~ if( !*src )
            //~ return;
        //~ dest++;
        //~ src++;
    //~ }
//~ }

static unsigned mystrlen(const char* s){
    unsigned len=0;
    while(*s){
        len++;
        s++;
    }
    return len;
}

//~ static unsigned adler32(const char* c){
    //~ //adler32 checksum
    //~ unsigned short v1=1;
    //~ unsigned short v2=0;
    //~ const unsigned char* p = (const unsigned char*) c;
    //~ while(*p){
        //~ v1 = (v1 + *p) % 65521;
        //~ v2 = (v2 + v1) % 65521;
        //~ p++;
    //~ }
    //~ unsigned sum = v2;
    //~ sum <<= 16;
    //~ sum |= v1;
    //~ //unsigned sum = (v2 << 16) | v1;
    //~ return sum;
//~ }

void sweet(){
    unsigned a6len = mystrlen(article6);
    unsigned a5len = mystrlen(article5);

    if( a6len != 949 ){
        kprintf("testsuite: article6 had wrong length: %d\n",a6len );
        return;
    }
    if( a5len != 850 ){
        kprintf("testsuite: article5 had wrong length: %d\n",a5len );
        return;
    }

    //~ if( adler32(article6) != 2129745098 )
    //~ {
        //~ kprintf("testsuite: article6 sum mismatch\n");
        //~ return;
    //~ }
    //~ if( adler32(article5) != 1624781340 ){
        //~ kprintf("testsuite: article5 sum mismatch\n");
        //~ return;
    //~ }

    vprintf("testsuite: Opening article6.txt\n");
    int article6fd1 = file_open("article6.txt", 0);
    if( article6fd1 != 0 ){
        kprintf("testsuite: Could not open article6.txt: %d\n",article6fd1);
        return;
    }
    else{
        vprintf("testsuite: Got %d\n",article6fd1);
    }

    if( article6fd1 < 0 ){
        kprintf("testsuite: Negative file descriptor from article6.txt");
        return;
    }

    vprintf("testsuite: Opening article5.txt\n");
    int article5fd = file_open("article5.txt", 0);
    if( article5fd < 0 ){
        kprintf("testsuite: Could not open article5.txt: %d\n",article5fd);
        return;
    }
    else{
        vprintf("testsuite: Got %d\n",article5fd);
    }

    vprintf("testsuite: Opening article6.txt a second time\n");
    int article6fd2 = file_open("article6.txt", 0);
    if( article6fd2 < 0 ){
        kprintf("testsuite: Could not open article6.txt a second time: %d\n",article6fd2);
        return;
    }

    if( article6fd1 == article6fd2 || article6fd1 == article5fd || article6fd2 == article5fd ){
        kprintf("File descriptors are not all different\n");
        return;
    }

    #define BUFFSIZE 1024
    static char a6a[BUFFSIZE];
    static char a6b[BUFFSIZE];
    static char a5[BUFFSIZE];

    char* p[3] = {a6a,a5,a6b};
    int fds[3] = {article6fd1, article5fd, article6fd2};
    unsigned count[3]={0,0,0};
    char canary = 0x42;
    unsigned readsize=27;
    int order[3]={0,1,2};
    while( 1 ){
        int atEOF=0;
        int tmp = order[0];
        order[0]=order[1];
        order[1]=order[2];
        order[2]=tmp;
        for(int j=0;j<3;++j){
            int i=order[j];
            p[i][count[i]+readsize+1] = canary;
            vprintf("testsuite: Calling file_read( %d, %p, %d)\n",fds[i],p[i]+count[i],readsize);
            int rv = file_read(fds[i], p[i]+count[i], readsize);
            vprintf("testsuite: Got %d\n",rv);
            if( rv < 0 ){
                kprintf("testsuite: Error in read\n");
                return;
            }
            if( rv > readsize ){
                kprintf("testsuite: Too much data read: Got %d, expected %d or less\n",rv,readsize);
                return;
            }
            count[i] += (unsigned)rv;
            if( rv == 0 )
                atEOF++;
            if( count[i] > BUFFSIZE-readsize ){
                kprintf("testsuite: Did not reach EOF in time");
                return;
            }
        }
        if(atEOF == 3 )
            break;
    }

    if( count[0] != a6len ){
        kprintf("testsuite: Read too much data for article6.txt (first fd): Got %d, expected %d\n", count[0], a6len );
        return;
    }
    if( count[2] != a6len ){
        kprintf("testsuite: Read too much data for article6.txt (second fd): Got %d, expected %d\n", count[0], a6len );
        return;
    }
    if( count[1] != a5len ){
        kprintf("testsuite: Read too much data for article5.txt: Got %d, expected %d\n", count[0], a5len );
        return;
    }

    if(VERBOSE){
        vprintf("testsuite: Contents of article6 (first time):\n");
        memdump(a6a,(int)a6len);
    }


    for(int i=0;i<a6len;++i){
        if( a6a[i] != article6[i] ){
            kprintf("Mismatch in article6.txt\n");
            return;
        }
    }

    if(VERBOSE){
        vprintf("testsuite: Contents of article6 (second time):\n");
        memdump(a6b,(int)a6len);
    }

    for(int i=0;i<a6len;++i){
        if( a6b[i] != article6[i] ){
            kprintf("testsuite: Mismatch in article6.txt\n");
            return;
        }
    }


    if(VERBOSE){
        vprintf("testsuite: Contents of article5:\n");
        memdump(a5,(int)a6len);
    }

    for(int i=0;i<a5len;++i){
        if( a5[i] != article5[i] ){
            kprintf("testsuite: Mismatch in article5.txt\n");
            return;
        }
    }

    int fd = article5fd;
    if( file_seek(fd,0,2) != 0 ){
        kprintf("file_seek reported error\n");
        return;
    }
    unsigned size;
    if( file_tell( fd, &size ) != 0 ){
        kprintf("file_tell reported error\n");
        return;
    }
    if( size != a5len ){
        kprintf("testsuite: Seek to end: Reported size as %d but expected %d\n", size,a5len);
        return;
    }

    unsigned expectedOffset = a5len;

    for(int i=0;seeks[i].whence != -1 ; i++ ){
        int delta = seeks[i].delta;
        int whence = seeks[i].whence;
        int rv = file_seek(fd,delta,whence);
        vprintf("testsuite: file_seek(%d, %d, %d) -> %d\n", fd,delta,whence,rv);
        unsigned newOffset=0x12345;
        if( file_tell(fd,&newOffset) != 0 ){
            kprintf("file_tell returned error\n");
            return;
        }
        long long xo=expectedOffset;
        if( whence == 0 )
            xo = delta;
        else if( whence == 1 )
            xo += delta;
        else if( whence == 2 ){
            xo = a5len;
            xo += delta;
        }
        if( xo < 0 || xo > 0xffffffff){
            if( rv == 0 ){
                kprintf("testsuite: file_seek succeeded but it should have failed\n");
                return;
            }
        } else {
            expectedOffset = (unsigned) xo;
        }

        if( newOffset != expectedOffset ){
            kprintf("testsuite: file_tell gave incorrect offset: Got %d, expected %d\n",
                newOffset,expectedOffset);
        }

        static char buf[32];
        for(int j=0;j<32;++j){
            buf[j]=0x42;
        }
        vprintf("testsuite: file_read(%d,%p,%d)\n",fd,buf,23);
        rv = file_read(fd,buf,23);
        vprintf("testsuite: file_read returned %d\n",rv);
        if( rv < 0 ){
            kprintf("testsuite: After seeking, file_read gave error %d\n",rv);
            return;
        }

        if( rv > 23 ){
            kprintf("testsuite: file_read: Read too much\n");
            return;
        }

        if(VERBOSE){
            vprintf("testsuite: After file_read: Buffer has:\n");
            memdump(buf,32);
        }

        for(int j=rv;j<32;++j){
            if( buf[j] != 0x42 ){
                kprintf("testsuite: file_read: Buffer overflow at %d\n",j);
                return;
            }
        }

        unsigned maxData;
        if( expectedOffset >= a5len )
            maxData = 0;
        else
            maxData = a5len-expectedOffset;

        if( rv > maxData ){
            kprintf("testsuite: file_read gave %d bytes, but it should have returned no more than %d\n",
                    rv, maxData );
            return;
        }

        vprintf("testsuite: Got data: ");
        for(int j=0;j<rv;++j){
            vprintf("%c",buf[j]);
        }
        vprintf("\n");

        for(unsigned j=0;j<(unsigned)rv;++j){
            if( buf[j] != article5[expectedOffset+j] ){
                kprintf("testsuite: file_read: data mismatch\n");
                return;
            }
        }

        expectedOffset += (unsigned)rv;
    }

    if( file_tell( fd, 0 ) >= 0 ){
        kprintf("file_tell didn't catch bad pointer\n");
        return;
    }

    kprintf("\n\n");

    //!!
    kprintf("All OK\n");
    //!!

}
