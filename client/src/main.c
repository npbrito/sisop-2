#include <stdbool.h>
#include <stdlib.h>
#include "error.h"
#include "command.h"
#include "tcputil.h"
#include "user.h"


int main(int argc, char* argv[argc+1])
{
    if (argc != 4) 
        err_quit("usage: ./myClient <username> <hostname/IPaddress> <service/port#>\n");
    
    user_t user = save_user(argv[1]);
    setup_user(user);

    int sockfd = Tcp_connect(argv[2], argv[3]);
    send_command(sockfd, argv[1]);   // Send username
    while (true) {
        char* cmd = read_command();
        parse_command(cmd, sockfd);
        free(cmd);
    }

    return EXIT_SUCCESS;
}