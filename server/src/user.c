#include <string.h>
#include "wrapper.h"
#include "user.h"
#include "packet.h"
#include "dir.h"

pthread_mutex_t mutex;

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

    return user;
}

void get_sync_dir(user_t user)
{
    Pthread_mutex_lock(&mutex);

    if (!check_dir_exists(user.dir))
        create_user_dir(user.username);

    Pthread_mutex_unlock(&mutex);
}