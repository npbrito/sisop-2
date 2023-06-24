#ifndef WRAPPER_H
#define WRAPPER_H
#include <stdio.h>
#include <stdlib.h>

// stdio.h functions

ssize_t Getline(char** lineptr, size_t* n, FILE* stream);


// Unix System Calls

void Close(int fd);
void* Malloc(size_t size);

#endif