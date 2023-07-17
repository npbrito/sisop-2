#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include "wrapper.h"
#include "user.h"
#include "packet.h"
#include "dir.h"
#include "command.h"

user_t recv_user(int sockfd)
{
    packet_t packet = recv_packet(sockfd);
    size_t len = strlen(packet.data) + 1;   // + 1 because of '\0'

    user_t user = {
        .username = Malloc(len),
        .dir = Malloc(strlen(SYNC_DIR) + len)
    };

    strncpy(user.username, packet.data, len);
    free(packet.data);
    strncpy(user.dir, SYNC_DIR, strlen(SYNC_DIR) + 1);              // + 1 because of '\0'
    strncat(user.dir, user.username, strlen(user.username) + 1);    // + 1 because of '\0'
    strncat(user.dir, "/", 2); // Adding / to be directory

    return user;
}

void sync_dir(user_t *user, int sockfd)
{
    /*
     When auth client, client remove itens on local sync_dir itself

    1. list server sync_dir
    2. foreach file in server sync_dir
        3. cmd_upload(filename, socket ...)
    */
    printf("on sync_dir\n");

    char path[256];
    DIR *dir;
    struct dirent *dp;

    strncpy(path, "./", 3);
    strncat(path, user->dir, strlen(user->dir) + 1);
    printf("path = %s;\n", path);
    dir = opendir(path);
    printf("dir = opendir(path);\n");

    while ((dp = readdir(dir)) != NULL)
    {
        // Ignore special dir
        if (strncmp(dp->d_name, ".", 1) == 0 || strncmp(dp->d_name, "..", 2) == 0)
            continue;

        // filename = dp->d_name
        send_upload(dp->d_name, user, sockfd);
    }
    fprintf(stdout, "get_sync_dir complete on %s\n", user->username);
}

void get_sync_dir(user_t *user, int sockfd)
{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    printf("On get_sync_dir\n");
    Pthread_mutex_lock(&mutex);

    if (!check_dir_exists(user->dir))
        create_user_dir(user->username);

    Pthread_mutex_unlock(&mutex);

    printf("before sync_dir\n");
    sync_dir(user, sockfd);
    printf("after sync_dir\n");
}