#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "util.h"
#include "wrapinet.h"
#include "wrappthread.h"
#include "wrapsock.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "tcputil.h"
#include "sockutil.h"
#include "packet.h"
#include "user.h"
#include "command.h"

static void *handler(void *arg);

int main(int argc, char* argv[argc+1])
{
    int listenfd;

    if (argc == 1)  // Use any free port
        listenfd = Tcp_listen(NULL); 
    else if (argc == 2)
        listenfd = Tcp_listen(argv[1]);
    else
        err_quit("usage: ./myServer [ <service/port#> ]\n");

    print_server(listenfd);

    while (true) {
        conndata_t* conndata = accept_connection(listenfd);
        handle_connection(conndata, &handler);
    }

    return EXIT_SUCCESS;
}

static void* handler(void* arg)
{
    conndata_t conndata = *(conndata_t *)arg;
    free(arg);
    Pthread_detach(pthread_self());

    print_client(conndata.cliaddr);
    user_t user = recv_user(conndata.connfd);
    setup_user(user);

    while (true) {
        char* cmd = recv_command(conndata.connfd);
        printf("Command: %s", cmd);
        free(cmd);
    }

    Close(conndata.connfd);
    
    return NULL;
}