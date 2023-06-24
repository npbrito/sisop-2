#include <string.h>
#include "command.h"
#include "packet.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "error.h"

char* read_command()
{
    size_t len = MAXLINE * sizeof(char);
    char* buff = Malloc(len);
    Getline(&buff, &len, stdin);

    return buff;
}

void send_command(int sockfd, char* str)
{
    packet_t packet = {
        .type = 1,  // CMD
        .seqn = 1,
        .max_seqn = 1,
        .data_length = strlen(str) + 1,  // str + '\0',
        .data = str
    };

    send_packet(sockfd, packet);
}