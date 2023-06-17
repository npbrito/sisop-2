#ifndef WRAPSOCK_H
#define WRAPSOCK_H
#include <sys/socket.h>
#define SA struct sockaddr

int Socket(int family, int type, int protocol);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

#endif