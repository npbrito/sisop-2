#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <string.h>
#include <limits.h>
#include "wrapper.h"
#include "error.h"
#include "command.h"
#include "tcputil.h"
#include "user.h"

#define BUFSIZE (100 * sizeof(struct inotify_event) + NAME_MAX + 1)

user_t user;

static void *file_system_listener(void *arg);
// static void *server_listener(void *arg);

int main(int argc, char *argv[argc + 1])
{
    if (argc != 5)
        err_quit("usage: ./myClient <username> <device_id> <hostname/IPaddress> <service/port#>\n");

    int cmdsockfd = Tcp_connect(argv[3], argv[4]);
    send_command(cmdsockfd, argv[1]); // Send username
    send_command(cmdsockfd, argv[2]); // Send device_id
    send_command(cmdsockfd, "1");     // Send client thread id
    int has_auth = recv_device_auth(cmdsockfd);

    if (!has_auth)
        err_quit("Too many devices connected for user %s", argv[1]);

    user = save_user(argv[1]);
    // TODO: get server info
    get_sync_dir(user);

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

    while (true)
    {
        char *cmd = read_command();
        parse_command(cmd, user.dir, cmdsockfd);
        free(cmd);
    }

    return EXIT_SUCCESS;
}

static void *file_system_listener(void *arg)
{
    int sockfd = *(int *)arg;
    free(arg);
    Pthread_detach(pthread_self());
    char dir[] = "./sync_dir/";
    int nfd = inotify_init();
    inotify_add_watch(nfd, dir, IN_CREATE | IN_DELETE | IN_MODIFY);
    char buf[BUFSIZE];

    while (true)
    {
        ssize_t len = read(nfd, buf, sizeof buf);
        struct inotify_event *event;

        for (char *ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len)
        {

            event = (struct inotify_event *)ptr;
            char cmd_arg[MAXLINE];

            if (event->mask & IN_CREATE)
            {
                strcat(cmd_arg, dir);
                strcat(cmd_arg, event->name);
                cmd_upload(sockfd, cmd_arg, user.dir);
            }

            if (event->mask & IN_DELETE)
            {
                strcat(cmd_arg, event->name);
                cmd_delete(sockfd, cmd_arg, user.dir);
            }

            if (event->mask & IN_MODIFY)
            {
                strcat(cmd_arg, event->name);
                cmd_delete(sockfd, cmd_arg, user.dir);
                cmd_arg[0] = '\0';
                strcat(cmd_arg, dir);
                strcat(cmd_arg, event->name);
                cmd_upload(sockfd, cmd_arg, user.dir);
            }

            cmd_arg[0] = '\0';

            // char cmd[] = "list_server";
            // char cmd[] = "upload ../../client/sync_dir/";
            // strcat(cmd, event->name);
            // printf("%s", cmd);
            // parse_command(cmd, sockfd);
        }
    }

    return NULL;
}

// static void *server_listener(void *arg)
// {
//     int sockfd = *(int *)arg;
//     free(arg);
//     Pthread_detach(pthread_self());

//     return NULL;
// }