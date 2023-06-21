#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include "error.h"
#include "util.h"
#include "wrapinet.h"
#include "wrappthread.h"
#include "wrapsock.h"
#include "wrapstdio.h"
#include "wrapunix.h"

typedef struct packet
{
    uint32_t type;
    uint32_t seqn;
    uint32_t max_seqn;
    uint32_t data_length;
    char *data;
} packet_t;

typedef struct conndata
{
    struct sockaddr_in cliaddr;
    int connfd;
} conndata_t;

static void *doit(void *arg);

int main()
{
    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = 0;  // Allocate port dynamically 
    Bind(listenfd, (SA *)&servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);
    socklen_t len = sizeof servaddr;
    getsockname(listenfd, (SA*)&servaddr, &len);
    printf("Server listening on port %d\n", ntohs(servaddr.sin_port));
    // TODO: Get current IP address in a portable way

    while (true)
    {
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof cliaddr;
        int connfd = Accept(listenfd, (SA*) &cliaddr, &len);
        conndata_t* conndata = Malloc(sizeof(conndata_t));
        conndata->cliaddr = cliaddr;
        conndata->connfd = connfd;
        pthread_t tid;
        Pthread_create(&tid, NULL, &doit, conndata);
    }

    return EXIT_SUCCESS;
}

static void *doit(void *arg)
{
    conndata_t conndata = *(conndata_t *)arg;
    free(arg);
    Pthread_detach(pthread_self());
    char buff[MAXLINE];
    printf("Connection from IP: %s, PORT: %d\n",
           Inet_ntop(AF_INET, &conndata.cliaddr.sin_addr, buff, sizeof buff),
           ntohs(conndata.cliaddr.sin_port));

    while (true) {
        ssize_t n;
        packet_t packet;

        if ( (n = Readn(conndata.connfd, &packet, 4*sizeof(uint32_t))) == 0)
            goto cleanup; // Client closed the connection

        packet.data = Malloc(packet.data_length);

        if ( (n = Readn(conndata.connfd, packet.data, packet.data_length)) == 0)
            goto cleanup; // Client closed the connection

        Fputs(packet.data, stdout);
        free(packet.data);
    }

cleanup:
    Close(conndata.connfd);
    return NULL;
}