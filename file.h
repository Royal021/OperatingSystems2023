#pragma once

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define MAX_PATH 64


int file_open(const char* fname, int flags);
int file_close(int fd);
int file_read(int fd, void* buf, unsigned count);
int file_write(int fd, void* buf, unsigned count);
int file_seek(int fd, int offset, int whence);
int file_tell(int fd, unsigned* offset);
int file_read_fully(int fd, void* buf, unsigned count);
