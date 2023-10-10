#pragma once
#include "fs.h"
#include "errno.h"

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#define O_CREAT  64
#define O_EXCL   128
#define O_TRUNC  512
#define O_APPEND 1024

int file_open(const char* fname, int flags);
int file_close(int fd);
int scanForMatchingFilename(char* fname, struct DirEntry ents[]);