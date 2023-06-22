#ifndef TCPUTIL_H
#define TCPUTIL_H
#include "wrapsock.h"

int Tcp_listen(char const* host, char const* serv, socklen_t* addrlenp);

#endif