#include <stdlib.h>
#include "wrapinet.h"
#include "error.h"

const char* Inet_ntop(int family, const void* addrptr, char* strptr, size_t len)
{
	const char* ptr;

	if ( (ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
		err_sys("inet_ntop error");

	return ptr;
}

void Inet_pton(int family, const char *strptr, void *addrptr)
{
	int	n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
		err_sys("inet_pton error for %s", strptr);	// errno set
	else if (n == 0)
		err_quit("inet_pton error for %s", strptr);	// errno not set
}