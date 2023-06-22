#ifndef SOCKUTIL_H
#define SOCKUTIL_H
#include <arpa/inet.h>

char* Sock_ntop(struct sockaddr const* sa, socklen_t salen);

#endif