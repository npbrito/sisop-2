#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include "tcputil.h"

int main(int argc, char* argv[argc+1])
{
    if (argc != 4) {
        fprintf(stderr, "usage: ./myClient <username> <hostname/IPaddress> <service/port#>\n");
        return EXIT_FAILURE;
    }
    
    int sockfd = Tcp_connect(argv[2], argv[3]);
    sendcmd(sockfd, argv[1]);   // Send username

    while (true) {
        char* cmd = readcmd();
        sendcmd(sockfd, cmd);
        free(cmd);
    }

    return EXIT_SUCCESS;
}