#ifndef WRAPINET_H
#define WRAPINET_H

#include <arpa/inet.h>

const char* Inet_ntop(int family, const void* addrptr, char* strptr, size_t len);
void Inet_pton(int family, const char* strptr, void* addrptr);

#endif