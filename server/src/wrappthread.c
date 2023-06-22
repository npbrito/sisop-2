#include <errno.h>
#include "error.h"
#include "wrappthread.h"

void Pthread_create(pthread_t* tid, const pthread_attr_t* attr, void* (*func)(void*), void* arg)
{
	int	n;

	if ( (n = pthread_create(tid, attr, func, arg)) == 0)
		return;

	errno = n;
	err_sys("pthread_create error");
}

void Pthread_detach(pthread_t tid)
{
	int	n;

	if ( (n = pthread_detach(tid)) == 0)
		return;
		
	errno = n;
	err_sys("pthread_detach error");
}

void Pthread_mutex_init(pthread_mutex_t* mutex, pthread_mutexattr_t *attr)
{
	int	n;

	if ( (n = pthread_mutex_init(mutex, attr)) == 0)
		return;

	errno = n;
	err_sys("pthread_create error");
}

void Pthread_mutex_destroy(pthread_mutex_t* mutex)
{
	int	n;

	if ( (n = pthread_mutex_destroy(mutex)) == 0)
		return;
		
	errno = n;
	err_sys("pthread_detach error");
}