#ifndef WRAPSTDIO_H
#define WRAPSTDIO_H
#include <stdio.h>

ssize_t Getline(char** lineptr, size_t* n, FILE* stream);

#endif