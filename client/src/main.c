#include <string.h>
#include <sys/types.h>
#include "util.h"
#include "wrapinet.h"
#include "wrapsock.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "error.h"

typedef struct packet {
    uint32_t type;
    uint32_t seqn;
    uint32_t max_seqn;
    uint32_t data_length;
    char* data;
} packet_t;

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
    Connect(sockfd, (SA*) &servaddr, sizeof servaddr);

    char sendline[MAXLINE+1];
    Fgets(sendline, MAXLINE, stdin);
    packet_t packet;
    packet.type = 0;
    packet.seqn = 1;
    packet.max_seqn = 1;
    packet.data_length = strlen(sendline)+1;
    Writen(sockfd, &packet, 4*sizeof(uint32_t));
    Writen(sockfd, sendline, packet.data_length);

    return EXIT_SUCCESS;
}