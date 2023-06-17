#ifndef WRAPUNIX_H
#define WRAPUNIX_H
#include <stdlib.h>
#include <sys/types.h>

ssize_t Read(int fd, void* ptr, size_t nbytes);

#endif