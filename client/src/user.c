#include <string.h>
#include <pthread.h>
#include "user.h"
#include "packet.h"
#include "wrapper.h"
#include "dir.h"

pthread_mutex_t mutex;

user_t save_user(char * username)
{
    size_t len = strlen(username) + 1;   // + 1 because of '\0'

    user_t user = {
        .username = Malloc(len),
        .dir = Malloc(strlen(SYNC_DIR) + len)
    };

    strncpy(user.username, username, len);
    strncpy(user.dir, SYNC_DIR, strlen(SYNC_DIR) + 1);              // + 1 because of '\0'
    strncat(user.dir, user.username, strlen(user.username) + 1);    // + 1 because of '\0'
    strncat(user.dir, "/", 2); // Adding / to be directory

    return user;
}

void get_sync_dir(user_t user)
{
    Pthread_mutex_lock(&mutex);

    if (!check_dir_exists(user.dir))
        create_user_dir(user.username);

    Pthread_mutex_unlock(&mutex);
}