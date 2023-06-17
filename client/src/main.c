#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "wrapinet.h"
#include "wrapsock.h"

int main(int argc, char* argv[argc+1])
{
    if (argc != 3) {
        fprintf(stderr, "usage: ./myClient <server_ip_address> <port>\n");
        return EXIT_FAILURE;
    }

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(str_to_port(argv[2]));
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    return EXIT_SUCCESS;
}