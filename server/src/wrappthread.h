#ifndef WRAPPTHREAD_H
#define WRAPPTHREAD_H

#include <pthread.h>

void Pthread_create(pthread_t* tid, const pthread_attr_t* attr, void* (*func)(void*), void* arg);
void Pthread_detach(pthread_t tid);

#endif