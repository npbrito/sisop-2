#ifndef WRAPSTDIO_H
#define WRAPSTDIO_H
#include <stdio.h>

char* Fgets(char* ptr, int n, FILE* stream);
void Fputs(const char*, FILE*);

#endif