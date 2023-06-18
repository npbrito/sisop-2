#ifndef WRAPUNIX_H
#define WRAPUNIX_H
#include <stdlib.h>
#include <sys/types.h>

void Close(int fd);
ssize_t Read(int fd, void* ptr, size_t nbytes);
void Write(int fd, void* ptr, size_t nbytes);

#endif