#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include "dir.h"
#include "error.h"
#include "util.h"
#include "wrapinet.h"
#include "wrappthread.h"
#include "wrapsock.h"
#include "wrapstdio.h"
#include "wrapunix.h"

#define h_addr h_addr_list[0]

pthread_mutex_t mutex;
 
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

int main(int argc, char *argv[argc + 1])
{
    if (argc != 1)
    {
        fprintf(stderr, "usage: ./myServer\n");
        return EXIT_FAILURE;
    }

    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Allocate port dynamically 
    servaddr.sin_port = 0;
    Bind(listenfd, (SA *)&servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);

    socklen_t len = sizeof servaddr;
    getsockname(listenfd, (SA *)&servaddr, &len);

    // TODO: Get current IP address

    fprintf(stdout, "Server listen on port %d\n", ntohs(servaddr.sin_port));

    while (true)
    {
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof cliaddr;
        int connfd = Accept(listenfd, (SA *)&cliaddr, &len);

        conndata_t *conndata = Malloc(sizeof(conndata_t));
        conndata->cliaddr = cliaddr;
        conndata->connfd = connfd;
        pthread_t tid;
        Pthread_mutex_init(&mutex, NULL);
        Pthread_create(&tid, NULL, &doit, conndata);
        Pthread_mutex_destroy(&mutex);
    }

    return EXIT_SUCCESS;
}

static void *doit(void *arg)
{
    conndata_t conndata = *(conndata_t *)arg;
    free(arg);
    Pthread_detach(pthread_self());
    char buff[MAXLINE];
    printf("connection from %s, port %d\n",
           Inet_ntop(AF_INET, &conndata.cliaddr.sin_addr, buff, sizeof buff),
           ntohs(conndata.cliaddr.sin_port));
    ssize_t n;
    packet_t packet;
    if ((n = Readn(conndata.connfd, &packet, 4 * sizeof(uint32_t))) == 0)
        goto cleanup; // Connection closed by other end

    packet.data = Malloc(packet.data_length);

    if ((n = Readn(conndata.connfd, packet.data, packet.data_length)) == 0)
        goto cleanup; // Connection closed by other end

    // Fputs(packet.data, stdout);

    // TODO: Check if first access to this thread
    user_t *user = malloc(sizeof (struct user_t*));
    user->username = malloc( sizeof (packet.data));
    user->username = packet.data;
    user->dir = malloc(strlen(SYNC_DIR) + strlen(packet.data) + 1);
    if(!checkDirExists(user->dir)){
        pthread_mutex_lock(&mutex);
        createUserDir(user->username);
        pthread_mutex_unlock(&mutex);
    }

    free(packet.data);

cleanup:
    Close(conndata.connfd);
    return NULL;
}