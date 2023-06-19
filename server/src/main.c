#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "error.h"
#include "util.h"
#include "wrapinet.h"
#include "wrappthread.h"
#include "wrapsock.h"
#include "wrapunix.h"

typedef struct conndata {
    struct sockaddr_in cliaddr;
    int connfd;
} conndata_t;


static void* doit(void* arg);

int main(int argc, char* argv[argc+1])
{
    if (argc != 2) {
        fprintf(stderr, "usage: ./myServer <port>\n");
        return EXIT_FAILURE;
    }

    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(str_to_port(argv[1]));
    Bind(listenfd, (SA*) &servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);

    while (true) {
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

static void* doit(void* arg)
{
    conndata_t conndata = *(conndata_t*) arg;
    free(arg);
    Pthread_detach(pthread_self());
    char buff[MAXLINE];
    printf("connection from %s, port %d\n",
           Inet_ntop(AF_INET, &conndata.cliaddr.sin_addr, buff, sizeof buff),
           ntohs(conndata.cliaddr.sin_port));
    time_t ticks = time(NULL);
    snprintf(buff, sizeof buff, "%.24s\r\n", ctime(&ticks));
    Write(conndata.connfd, buff, strlen(buff));
    Close(conndata.connfd);
    return NULL;
}