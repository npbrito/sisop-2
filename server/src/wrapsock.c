#include <errno.h>
#include "wrapsock.h"
#include "error.h"

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

void Listen(int fd, int backlog)
{
	if (listen(fd, backlog) < 0)
		err_sys("listen error");
}

int Socket(int family, int type, int protocol)
{
	int	n;

	if ( (n = socket(family, type, protocol)) < 0)
		err_sys("socket error");

	return n;
}

