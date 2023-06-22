#include <stdbool.h>
#include <stdlib.h>
#include "error.h"
#include "command.h"
#include "tcputil.h"

int main(int argc, char* argv[argc+1])
{
    if (argc != 4) 
        err_quit("usage: ./myClient <username> <hostname/IPaddress> <service/port#>\n");
    
    int sockfd = Tcp_connect(argv[2], argv[3]);
    sendcmd(sockfd, argv[1]);   // Send username

    while (true) {
        char* cmd = readcmd();
        sendcmd(sockfd, cmd);
        free(cmd);
    }

    return EXIT_SUCCESS;
}