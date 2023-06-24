#include <unistd.h>
#include "error.h"
#include "wrapper.h"

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