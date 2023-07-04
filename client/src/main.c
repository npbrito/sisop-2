#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "wrapper.h"
#include "error.h"
#include "command.h"
#include "tcputil.h"

static void *file_system_listener(void *arg);
//static void *server_listener(void *arg);

int main(int argc, char* argv[argc+1])
{
    if (argc != 5) 
        err_quit("usage: ./myClient <username> <device_id> <hostname/IPaddress> <service/port#>\n");
    
    int cmdsockfd = Tcp_connect(argv[3], argv[4]);
    send_command(cmdsockfd, argv[1]);   // Send username
    send_command(cmdsockfd, argv[2]);   // Send device_id
    send_command(cmdsockfd, "1");       // Send client thread id
    int has_auth = recv_device_auth(cmdsockfd);

    if (!has_auth)
        err_quit("Too many devices connected for user %s", argv[1]);

    // int *fssockfd = Malloc(sizeof(int));
    // *fssockfd = Tcp_connect(argv[3], argv[4]);
    // send_command(*fssockfd, argv[1]);   // Send username
    // send_command(*fssockfd, argv[2]);   // Send device_id
    // send_command(*fssockfd, "2");       // Send client thread id
    // pthread_t fstid;
    // Pthread_create(&fstid, NULL, &file_system_listener, &fssockfd);

    // int *servsockfd = Malloc(sizeof(int));
    // *servsockfd = Tcp_connect(argv[3], argv[4]);
    // send_command(*servsockfd, argv[1]);   // Send username
    // send_command(*servsockfd, argv[2]);   // Send device_id
    // send_command(*servsockfd, "3");       // Send client thread id
    // pthread_t servtid;
    // Pthread_create(&servtid, NULL, &server_listener, &servsockfd);

    while (true) {
        char* cmd = read_command();
        parse_command(cmd, cmdsockfd);
        free(cmd);
    }

    return EXIT_SUCCESS;
}

// static void *file_system_listener(void *arg)
// {
//     int sockfd = *(int *)arg;
//     free(arg);
//     Pthread_detach(pthread_self());

//     // Code for inotify, etc goes here!
//     // Monitor local file system events in sync_dir here!

//     return NULL;
// }

// static void *server_listener(void *arg)
// {
//     int sockfd = *(int *)arg;
//     free(arg);
//     Pthread_detach(pthread_self());

//     return NULL;
// }