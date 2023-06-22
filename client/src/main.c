#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include "command.h"
#include "tcputil.h"
#include "util.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "error.h"

int main(int argc, char *argv[argc+1])
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