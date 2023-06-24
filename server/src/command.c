#include <string.h>
#include "command.h"
#include "packet.h"
#include "wrapunix.h"
#include "error.h"

void send_command(int sockfd, char* str)
{
    packet_t packet = {
        .type = 0,  // CMD
        .seqn = 1,
        .max_seqn = 1,
        .data_length = strlen(str) + 1,  // str + '\0',
        .data = str
    };

    send_packet(sockfd, packet);
}

char* recv_command(int sockfd)
{
    packet_t packet = recv_packet(sockfd);
    char* buff = Malloc(strlen(packet.data) + 1); // str + '\0',
    strncpy(buff, packet.data, strlen(packet.data) + 1);
    free(packet.data);

    return buff;
}