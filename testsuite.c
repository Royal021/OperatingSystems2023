#include "errno.h"
#include "kprintf.h"
#include "file.h"

/* static const char* nonexist[] = {
        "blargh", "blargleblargleboom",
        "blargleblargleboom.burpblah", "b.burpblah",
        "b.b", "art.txt","article1234.txt",
        "article 2.TXT",
        "article1.T","article1txt",
        "article1.txtt", "bill..txt",
        "article10.txt", "article100.txt",
        "article1.txts","bills.txt",
        "bill    .txt", "bill    txt",
        "const.c ","const.c  ","const.c.",
        "const.c..","const.c...",
        "thisisareallylongfilenamebutitshouldnotcrashyouroperatingsystemifyourcodeiscorrectlywrittenandrobustlydesigned.c",
        "x", "", ".txt",
        0};
 */
static const char* exist[] = {
    "article1.txt", "ARTICLE2.txt",
    "ARTICLE3.TXT", "ArTiClE4.txt",
    "article5.TXT", "bill.txt",
    "CONST.C", 0
};

void sweet(){

   /*  kprintf("==================================\n");
    kprintf("Testing nonexistent file open...\n");
    kprintf("==================================\n");
    for(int i=0;nonexist[i];++i){
        kprintf("Trying to open %s (should fail)\n",nonexist[i]);
        int fd = file_open(nonexist[i],0);
        if( fd >= 0 ){
            kprintf("Succeeded, but it should have failed\n");
            return;
        } else {
            kprintf("Failed with code %d [OK!]\n",fd);
        }
    }

    kprintf("==================================\n");
    kprintf("Testing close of non-open files\n");
    kprintf("==================================\n");
    for(int i=-4;i<10;++i){
        kprintf("file_close(%d) [should fail]... ",i);
        int rv = file_close(i);
        if( rv >= 0 ){
            kprintf("Succeeded, but it should have failed\n");
            return;
        } else {
            kprintf("Failed with code %d [OK]\n",rv);
        }
    } */
    //first time
    kprintf("==================================\n");
    kprintf("Testing open of existing files\n");
    kprintf("==================================\n");
    int used[16];
    for(int j=0;j<16;++j)
        used[j] = -1;

    for(int i=0;exist[i];++i){
        const char* tmp=exist[i];
        kprintf("file_open(\"%s\") [should succeed]... ",tmp);
        int fd = file_open( tmp, 0 );
        if(fd<0){
            kprintf("Open failed with error %d\n",fd);
            return;
        }
        used[i]=fd;
        for(int j=0;j<i;++j){
            if( used[j] == used[i] ){
                kprintf("Duplicate fd (%d): Same fd was returned for two different files\n",
                    fd);
                return;
            }
        }
        kprintf("Succeeded with fd=%d [OK]\n",fd);
    }

    for(int i=15;i>=0;i--){
        if( used[i] != -1 ){
            kprintf("Testing file_close(%d) (should succeed)...\n",used[i]);
            int rv = file_close(used[i]);
            if(rv == 0 ){
                kprintf("Succeeded [OK]\n");
            } else {
                kprintf("Failed with error code %d\n",rv);
                return;
            }
        }
    }

    for(int i=15;i>=0;i--){
        if( used[i] != -1 ){
            kprintf("Testing file_close(%d) (should fail)...\n",used[i]);
            int rv = file_close(used[i]);
            if(rv == 0 ){
                kprintf("Succeeded, but it should have failed\n");
                return;
            } else {
                kprintf("Failed with error code %d [OK]\n",rv);
            }
        }
    }

    kprintf("==================================\n");
    kprintf("Exhaustion test...\n");
    kprintf("==================================\n");

    int rv;
    for(int i=0;i<1000;++i){
        rv = file_open("ARTICLE1.TXT",0);
        if( rv < 0 ){
            if( i < 16 ){
                kprintf("file_open() failed after only %d operations\n",i);
                return;
            }
        }
    }
    if( rv >= 0 ){
        kprintf("file_open never returned failure [Bad]\n");
        return;
    }

    //!!
    kprintf("All OK\n");
    //!!
}
