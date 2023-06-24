#ifndef UTIL_H
#define UTIL_H
#include <sys/types.h>

ssize_t Readn(int fd, void* ptr, size_t nbytes);
void Writen(int fd, void* ptr, size_t nbytes);

#endif