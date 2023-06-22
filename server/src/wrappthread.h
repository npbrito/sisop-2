#ifndef WRAPPTHREAD_H
#define WRAPPTHREAD_H

#include <pthread.h>

void Pthread_create(pthread_t* tid, const pthread_attr_t* attr, void* (*func)(void*), void* arg);
void Pthread_detach(pthread_t tid);
void Pthread_mutex_init(pthread_mutex_t* mutex, pthread_mutexattr_t *attr);
void Pthread_mutex_destroy(pthread_mutex_t* mutex);

#endif