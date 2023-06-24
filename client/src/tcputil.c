#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "tcputil.h"
#include "wrapunix.h"

int tcp_connect(char const* host, char const* serv)
{
	struct addrinfo	hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int n;
	struct addrinfo* res;
	struct addrinfo* ressave;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_connect error for %s, %s: %s", host, serv, gai_strerror(n));

	ressave = res;
	int sockfd;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		if (sockfd < 0)
			continue;

		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;		// Success

		Close(sockfd);	// Ignore this one. Connect error.
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	// errno set from final connect()
		err_sys("tcp_connect error for %s, %s", host, serv);

	freeaddrinfo(ressave);

	return sockfd;
}

int Tcp_connect(char const* host, char const* serv)
{
	return tcp_connect(host, serv);
}