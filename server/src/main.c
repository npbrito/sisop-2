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
#include "tcputil.h"
#include "sockutil.h"

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

int main(int argc, char* argv[argc+1])
{
    int listenfd;
    socklen_t addrlen;

    if (argc == 2)      // IPv6 Server on dual-stack host
        listenfd = Tcp_listen(NULL, argv[1], &addrlen); 
    else if (argc == 3) // IPv4 or IPv6 depending on host argument
        listenfd = Tcp_listen(argv[1], argv[2], &addrlen); 
    else
        err_quit("usage: ./myServer [ <host> ] <service/port#>\n");

    while (true)
    {
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof cliaddr;
        int connfd = Accept(listenfd, (SA*) &cliaddr, &len);
        printf("Connection from %s\n", Sock_ntop((SA*) &cliaddr, len));
        conndata_t* conndata = Malloc(sizeof(conndata_t));
        conndata->cliaddr = cliaddr;
        conndata->connfd = connfd;
        pthread_t tid;
        Pthread_create(&tid, NULL, &doit, conndata);
    }

    return EXIT_SUCCESS;
}

static void* doit(void* arg)
{
    conndata_t conndata = *(conndata_t*) arg;
    free(arg);
    Pthread_detach(pthread_self());

    while (true) {
        ssize_t n;
        packet_t packet;

        if ( (n = Readn(conndata.connfd, &packet, 4 * sizeof(uint32_t))) == 0)
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