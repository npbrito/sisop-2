#include <string.h>
#include "wrapper.h"
#include "user.h"
#include "packet.h"
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

void get_sync_dir(char *userdir, int sockfd)
{
    fprintf(stdout, "Synchronizing... \n");
    // char buff[MAXLINE];
    // char path[256];
    // pthread_mutex_t read_write_mutex = PTHREAD_MUTEX_INITIALIZER;
    // sprintf(buff, "sync %s", userdir);
    // send_command(sockfd, buff);

    // packet_t packet = recv_packet(sockfd);
    // int file_count = 0;
    // int file_count_max = atoi(packet.data);

    // while (file_count < file_count_max)
    // {
    //     // filename
    //     packet_t packet = recv_packet(sockfd);
    //     strcpy(path, userdir);
    //     strncat(path, packet.data, strlen(packet.data) + 1);
    //     Pthread_mutex_lock(&read_write_mutex);
    //     FILE *fileptr = fopen(path, "wb");
    //     if (fileptr == NULL)
    //         err_msg("failed to open file");

    //     while (packet.seqn < packet.max_seqn)
    //     {
    //         packet = recv_packet(sockfd);
    //         fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
    //     }

    //     fclose(fileptr);
    //     Pthread_mutex_unlock(&read_write_mutex);
    // }
    fprintf(stdout, "Sync complete. Ready to operate.\n");
}

void initialize(user_t user, int sockfd)
{
    Pthread_mutex_lock(&mutex);
    if (!check_dir_exists(user.dir))
        create_user_dir(user.username);
    Pthread_mutex_unlock(&mutex);

    get_sync_dir(user.dir, sockfd);
}