#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "wrapper.h"
#include "error.h"

// Unix functions

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

void Write(int fd, void* ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		err_sys("write error");
}

// Socket functions

int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr)
{
	int	n;

again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
		if (errno == ENETDOWN || 
			errno == EPROTO || 
			errno == ENOPROTOOPT || 
			errno == EHOSTDOWN || 
			errno == ENONET || 
			errno == EHOSTUNREACH || 
			errno == EOPNOTSUPP || 
			errno == ENETUNREACH)
			goto again;
		else
			err_sys("accept error");
	}

	return n;
}

void Bind(int fd, const struct sockaddr* sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		err_sys("bind error");
}

void Connect(int fd, const struct sockaddr* sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0)
		err_sys("connect error");
}

void Getsockname(int fd, struct sockaddr* sa, socklen_t* salenptr)
{
	if (getsockname(fd, sa, salenptr) < 0)
		err_sys("getsockname error");
}

void Listen(int fd, int backlog)
{
	if (listen(fd, backlog) < 0)
		err_sys("listen error");
}

void Setsockopt(int fd, int level, int optname, void const* optval, socklen_t optlen)
{
	if (setsockopt(fd, level, optname, optval, optlen) < 0)
		err_sys("setsockopt error");
}

int Socket(int family, int type, int protocol)
{
	int	n;

	if ( (n = socket(family, type, protocol)) < 0)
		err_sys("socket error");

	return n;
}

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

void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr)
{
	int	n;

	if ( (n = pthread_mutex_init(mptr, attr)) == 0)
		return;

	errno = n;
	err_sys("pthread_mutex_init error");
}

void Pthread_mutex_destroy(pthread_mutex_t *mptr)
{
	int	n;

	if ( (n = pthread_mutex_destroy(mptr)) == 0)
		return;
		
	errno = n;
	err_sys("pthread_mutex_destroy error");
}

void Pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int	n;

	if ( (n = pthread_mutex_lock(mptr)) == 0)
		return;

	errno = n;
	err_sys("pthread_mutex_lock error");
}

void Pthread_mutex_unlock(pthread_mutex_t *mptr)
{
	int	n;

	if ( (n = pthread_mutex_unlock(mptr)) == 0)
		return;

	errno = n;
	err_sys("pthread_mutex_unlock error");
}

// Inet functions

const char* Inet_ntop(int family, const void* addrptr, char* strptr, size_t len)
{
	const char* ptr;

	if ( (ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
		err_sys("inet_ntop error");

	return ptr;
}

void Inet_pton(int family, const char* strptr, void* addrptr)
{
	int	n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
		err_sys("inet_pton error for %s", strptr);	// errno set
	else if (n == 0)
		err_quit("inet_pton error for %s", strptr);	// errno not set
}