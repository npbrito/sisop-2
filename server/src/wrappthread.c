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