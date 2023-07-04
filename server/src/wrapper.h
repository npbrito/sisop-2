#ifndef WRAPPER_H
#define WRAPPER_H

#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#define SA struct sockaddr
#define	LISTENQ	1024    // 2nd argument to listen()

// Unix functions

void Close(int fd);
void* Malloc(size_t size);
ssize_t Read(int fd, void* ptr, size_t nbytes);
void Write(int fd, void* ptr, size_t nbytes);

// Socket functions

int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr);
void Bind(int fd, const struct sockaddr* sa, socklen_t salen);
void Connect(int fd, const struct sockaddr* sa, socklen_t salen);
void Getsockname(int fd, struct sockaddr* sa, socklen_t* salenptr);
void Listen(int fd, int backlog);
void Setsockopt(int fd, int level, int optname, void const* optval, socklen_t optlen);
int Socket(int family, int type, int protocol);

// pthread functions

void Pthread_create(pthread_t* tid, const pthread_attr_t* attr, void* (*func)(void*), void* arg);
void Pthread_detach(pthread_t tid);
void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr);
void Pthread_mutex_destroy(pthread_mutex_t *mptr);
void Pthread_mutex_lock(pthread_mutex_t *mptr);
void Pthread_mutex_unlock(pthread_mutex_t *mptr);

// Inet functions

const char* Inet_ntop(int family, const void* addrptr, char* strptr, size_t len);
void Inet_pton(int family, const char* strptr, void* addrptr);

#endif