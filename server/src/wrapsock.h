#ifndef WRAPSOCK_H
#define WRAPSOCK_H
#include <sys/socket.h>
#define SA struct sockaddr
#define	LISTENQ	1024    // 2nd argument to listen()

int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr);
void Bind(int fd, const struct sockaddr* sa, socklen_t salen);
void Connect(int fd, const struct sockaddr* sa, socklen_t salen);
void Listen(int fd, int backlog);
void Setsockopt(int fd, int level, int optname, void const* optval, socklen_t optlen);
int Socket(int family, int type, int protocol);

#endif