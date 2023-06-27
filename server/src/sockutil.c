#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "sockutil.h"
#include "wrapinet.h"
#include "wrapsock.h"
#include "wrappthread.h"
#include "wrapunix.h"

void print_server(int listenfd)
{
	struct sockaddr_in servaddr;
	socklen_t len = sizeof servaddr;
	Getsockname(listenfd, (SA*) &servaddr, &len);
	uint16_t port = ntohs(servaddr.sin_port);

	// Uses UDP Socket to get local network address
	int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in cliaddr;
	memset(&cliaddr, 0, sizeof cliaddr);
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = 0;
	Inet_pton(AF_INET, "8.8.8.8", &cliaddr.sin_addr); 
	Connect(sockfd, (SA *) &cliaddr, sizeof cliaddr);
	len = sizeof cliaddr;
	Getsockname(sockfd, (SA *) &cliaddr, &len);

	char buff[MAXLINE];
	printf("Server listening on IP: %s PORT: %hu\n", 
		   Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof buff),
		   port);
}

void print_client(struct sockaddr_in cliaddr)
{
	char buff[MAXLINE];
	printf("Client connecting from IP: %s PORT: %hu\n", 
		   Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof buff),
		   ntohs(cliaddr.sin_port));
}

conndata_t* accept_connection(int listenfd)
{
	struct sockaddr_in cliaddr;
    socklen_t len = sizeof cliaddr;
    int connfd = Accept(listenfd, (SA*) &cliaddr, &len);
    conndata_t* conndata = Malloc(sizeof(conndata_t));
    conndata->cliaddr = cliaddr;
    conndata->connfd = connfd;

	return conndata;
}

void handle_connection(connlist_t* connlist, void* (*handler)(void*))
{
	pthread_t tid;
    Pthread_create(&tid, NULL, handler, connlist);
}