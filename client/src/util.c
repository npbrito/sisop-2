#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "error.h"
#include "util.h"

// Read "n" bytes from a descriptor.
ssize_t	readn(int fd, void* vptr, size_t n)
{
	size_t nleft;
	ssize_t	nread;
	char* ptr;
	ptr = vptr;
	nleft = n;

	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		// and call read() again
			else
				return -1;      // error
		} else if (nread == 0)
			break;				// EOF 

		nleft -= nread;
		ptr   += nread;
	}

	return n - nleft;		// return >= 0 
}

ssize_t Readn(int fd, void* ptr, size_t nbytes)
{
	ssize_t	n;

	if ( (n = readn(fd, ptr, nbytes)) < 0)
		err_sys("readn error");

	return n;
}

// Write "n" bytes to a descriptor.
ssize_t	writen(int fd, const void* vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char* ptr;
	ptr = vptr;
	nleft = n;

	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		// and call write() again 
			else
				return -1;			// error 
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return n;
}

void Writen(int fd, void* ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_sys("writen error");
}