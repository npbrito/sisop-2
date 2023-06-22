#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "tcputil.h"
#include "wrapunix.h"

int tcp_listen(char const* host, char const* serv, socklen_t* addrlenp)
{
	struct addrinfo	hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    int n;
	struct addrinfo* res;
	struct addrinfo* ressave;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_listen error for %s, %s: %s", host, serv, gai_strerror(n));

	ressave = res;
	const int on = 1;
    int	listenfd;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		if (listenfd < 0)
			// Ignore this one. It is not a fatal error.
			// Could happen if an IPv6 address is returned 
			// but the host kernel does not support IPv6.
			continue;

		Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;			// Success

		Close(listenfd);	// Ignore this one. Bind error.
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	// errno from final socket() or bind()
		err_sys("tcp_listen error for %s, %s", host, serv);

	Listen(listenfd, LISTENQ);

	if (addrlenp)
		*addrlenp = res->ai_addrlen;	// Return size of protocol address

	freeaddrinfo(ressave);

	return listenfd;
}

int Tcp_listen(char const* host, char const* serv, socklen_t* addrlenp)
{
	return tcp_listen(host, serv, addrlenp);
}