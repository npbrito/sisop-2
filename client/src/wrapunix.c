#include <unistd.h>
#include "error.h"
#include "wrapunix.h"

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

ssize_t Read(int fd, void* ptr, size_t nbytes)
{
    ssize_t	n;

	if ( (n = read(fd, ptr, nbytes)) == -1)
		err_sys("read error");

	return n;
}