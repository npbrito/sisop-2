#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include "util.h"
#include "wrapinet.h"
#include "wrapsock.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "error.h"

#define h_addr h_addr_list[0]
typedef struct packet
{
    uint32_t type;
    uint32_t seqn;
    uint32_t max_seqn;
    uint32_t data_length;
    char *data;
} packet_t;

int main(int argc, char *argv[argc + 1])
{
    if (argc != 4)
    {
        fprintf(stderr, "usage: ./myClient <username> <server_ip_address> <port>\n");
        return EXIT_FAILURE;
    }
    struct hostent *server;
    server = gethostbyname(argv[2]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    fprintf(stdout, "%s", server->h_addr);

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(str_to_port(argv[3]));
    // Inet_pton(AF_INET, argv[2], &servaddr.sin_addr);
    servaddr.sin_addr = *((struct in_addr *)server->h_addr);
    Connect(sockfd, (SA *)&servaddr, sizeof servaddr);

    char username[strlen(argv[1]) + 1];
    strcpy(username, argv[1]);

    packet_t packet;
    packet.type = 0;
    packet.seqn = 1;
    packet.max_seqn = 1;
    packet.data_length = strlen(username) + 1;
    // Sending username to connect on server
    Writen(sockfd, &packet, 4 * sizeof(uint32_t));
    Writen(sockfd, username, packet.data_length);

    return EXIT_SUCCESS;
}