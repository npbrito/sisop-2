#ifndef WRAPPER_H
#define WRAPPER_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// pthread functions

void Pthread_create(pthread_t* tid, const pthread_attr_t* attr, void* (*func)(void*), void* arg);
void Pthread_detach(pthread_t tid);

// stdio.h functions

ssize_t Getline(char** lineptr, size_t* n, FILE* stream);

// Unix System Calls

void Close(int fd);
void* Malloc(size_t size);

#endif