#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "error.h"
#include "util.h"
#include "wrapsock.h"
#include "wrapunix.h"

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
        int connfd = Accept(listenfd, (SA*) NULL, NULL);
        time_t ticks = time(NULL);
        char buff[MAXLINE];
        snprintf(buff, sizeof buff, "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));
        Close(connfd);
    }

    return EXIT_SUCCESS;
}