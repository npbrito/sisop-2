#ifndef WRAPINET_H
#define WRAPINET_H

#include <arpa/inet.h>

void Inet_pton(int family, const char *strptr, void *addrptr);

#endif