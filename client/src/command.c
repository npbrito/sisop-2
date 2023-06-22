#include <string.h>
#include "command.h"
#include "packet.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "error.h"

char* readcmd()
{
    size_t len = MAXLINE * sizeof(char);
    char* buff = Malloc(len);
    Getline(&buff, &len, stdin);

    return buff;
}

void sendcmd(int sockfd, char* str)
{
    packet_t packet = {
        .type = 0,  // CMD
        .seqn = 1,
        .max_seqn = 1,
        .data_length = strlen(str) + 1,  // str + '\0',
        .data = str
    };

    sendpckt(sockfd, packet);
}