#ifndef SOCKUTIL_H
#define SOCKUTIL_H
#include <netinet/in.h>

#include "client.h"
#include "device.h"

typedef struct connlist {
    client_t* clients;
    conndata_t* conndata;
} connlist_t;

typedef struct conndata
{
    struct sockaddr_in cliaddr;
    int connfd;
} conndata_t;

conndata_t* accept_connection(int listenfd);
void handle_connection(conndata_t* conndata, void* (*handler)(void*));
void print_client(struct sockaddr_in cliaddr);
void print_server(int sockfd);

#endif