#include <errno.h>
#include <unistd.h>
#include "error.h"
#include "wrapper.h"

// pthread functions

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

// stdio.h functions

ssize_t Getline(char** lineptr, size_t* n, FILE* stream)
{
	ssize_t c;

	if ( (c = getline(lineptr, n, stream)) == -1)
		err_sys("getline error");

	return c;
}

// Unix System Calls

void Close(int fd)
{
	if (close(fd) == -1)
		err_sys("close error");
}

void* Malloc(size_t size)
{
	void *ptr;

	if ( (ptr = malloc(size)) == NULL)
		err_sys("malloc error");

	return ptr;
}