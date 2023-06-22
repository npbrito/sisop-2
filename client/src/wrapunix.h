#ifndef WRAPUNIX_H
#define WRAPUNIX_H
#include <stdlib.h>
#include <sys/types.h>

void Close(int fd);
void* Malloc(size_t size);

#endif