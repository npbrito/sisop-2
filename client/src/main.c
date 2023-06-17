#include <string.h>
#include "util.h"
#include "wrapinet.h"
#include "wrapsock.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "error.h"

int main(int argc, char* argv[argc+1])
{
    if (argc != 4) {
        fprintf(stderr, "usage: ./myClient <username> <server_ip_address> <port>\n");
        return EXIT_FAILURE;
    }

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(str_to_port(argv[3]));
    Inet_pton(AF_INET, argv[2], &servaddr.sin_addr);
    Connect(sockfd, (SA *) &servaddr, sizeof servaddr);

    char recvline[MAXLINE+1];
    ssize_t n;

    while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	// NULL terminate
        Fputs(recvline, stdout);
	}

    return EXIT_SUCCESS;
}