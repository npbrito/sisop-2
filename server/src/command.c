#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include "command.h"
#include "packet.h"
#include "wrapper.h"
#include "error.h"
#include "sockutil.h"
#include "util.h"
#include "math.h"

cmd_t dispatch_table[] = {
    CMD(upload, NULL, 1),
    CMD(download, NULL, 1),
    CMD(delete, NULL, 1),
    CMD(list_server, NULL, 0),
    CMD(exit, NULL, 0)};

void parse_command(char *cmdline, user_t *user, int sockfd)
{
    char const *delim = " \n";
    char *saveptr;
    char const *cmd = strtok_r(cmdline, delim, &saveptr);
    size_t num_cmds = sizeof dispatch_table / sizeof dispatch_table[0];

    for (int i = 0; i < num_cmds; i++)
    {
        cmd_t current_cmd = dispatch_table[i];

        if (!strcmp(cmd, current_cmd.name))
        {
            char const *arg = strtok_r(NULL, delim, &saveptr);
            current_cmd.func(arg, user, sockfd);
            return;
        }
    }
}

void send_command(int sockfd, char *str)
{
    packet_t packet = {
        .type = COMMAND,
        .seqn = 1,
        .max_seqn = 1,
        .data_length = strlen(str) + 1, // str + '\0',
        .data = str};

    send_packet(sockfd, packet);
}

void send_device_auth(int sockfd)
{
    Write(sockfd, "1", sizeof(char));
}

void progress_bar(float progress)
{
    int bar_width = 50;
    int filled = progress * bar_width;

    fprintf(stdout, "[");
    for (int i = 0; i < bar_width; i++)
    {
        if (i < filled)
            fprintf(stdout, "#");
        else
            fprintf(stdout, " ");
    }
    fprintf(stdout, "] %.2f%%\r", progress * 100.0);
    fflush(stdout);

    if (progress == 1.0)
        fprintf(stdout, "\n");
}

void propagate_upload(char const *arg, user_t const *user, int sockfd)
{
    client_t *client = get_client_by_user(clients, user->username);
    // Pthread_mutex_lock()
    int device_count = get_device_count(&(client->devices));
    device_t *current_device = &(client->devices);
    float download_progress = 0.0;

    for (int i = 0; i < device_count; i++)
    {
        FILE *fileptr;
        size_t file_size;
        char path[256];
        strcpy(path, user->dir);
        strncat(path, arg, strlen(arg) + 1);
        char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
        char cmd[MAXLINE];

        int seq = 1;
        int max_seq;

        if (!check_file_exists(path))
            err_msg("file does not exist");

        fileptr = fopen(path, "rb");
        if (fileptr == NULL)
            err_msg("failed to open file");

        // get file size
        fseek(fileptr, 0, SEEK_END);
        file_size = ftell(fileptr);
        rewind(fileptr);

        sprintf(cmd, "receive_upload %s", arg);
        send_command(current_device->servconn.connfd, cmd);

        max_seq = file_size / MAX_DATA_SIZE;

        size_t bufflen;
        fprintf(stdout, "Uploading: %s // Size: %ld // Num of packets: %ld\n", arg, file_size, file_size / MAX_DATA_SIZE);

        do
        {
            bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE - strlen("receive_upload "), fileptr);
            // upload progress
            download_progress = (float)ftell(fileptr) / file_size;
            progress_bar(download_progress);
            // Send custom packet with characters read
            packet_t packet = {
                .type = DATA,
                .seqn = seq,
                .max_seqn = max_seq,
                .data_length = bufflen,
                .data = buffer};

            Writen(current_device->servconn.connfd, &packet, 4 * sizeof(uint32_t));
            Writen(current_device->servconn.connfd, packet.data, packet.data_length);
            seq++;
        } while (!feof(fileptr) && bufflen > 0);

        fclose(fileptr);
        free(buffer);

        current_device = current_device->next;
    }
}

void propagate_delete(char const *arg, user_t const *user, int sockfd)
{
    client_t *client = get_client_by_user(clients, user->username);
    int device_count = get_device_count(&(client->devices));

    device_t *current_device = &(client->devices);

    for (int i = 0; i < device_count; i++)
    {
        char buff[MAXLINE];
        sprintf(buff, "receive_delete %s", arg);
        send_command(current_device->servconn.connfd, buff);
        current_device = current_device->next;
    }
}

void cmd_upload(char const *arg, user_t const *user, int sockfd)
{
    char path[265]; // 256 + sync_dir_
    strcpy(path, user->dir);
    strncat(path, arg, sizeof(path) - strlen(path) - 1);

    FILE *fileptr = fopen(path, "wb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    packet_t packet = {
        .type = DATA,
        .seqn = 0,
        .max_seqn = 1,
        .data_length = 0,
        .data = ""};

    while (packet.seqn <= packet.max_seqn)
    {
        packet = recv_packet(sockfd);
        fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
    }

    fclose(fileptr);
    fprintf(stdout, "File upload complete: %s\n", arg);
    propagate_upload(arg, user, sockfd);
}

void cmd_download(char const *arg, user_t const *user, int sockfd)
{
    FILE *fileptr;
    size_t file_size;
    char path[256];

    strcpy(path, user->dir);
    strncat(path, arg, strlen(arg) + 1);
    char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
    char cmd[MAXLINE];
    float download_progress = 0.0;


    // Verify if file exists
    if (!check_file_exists(path))
        err_msg("file does not exist");

    fileptr = fopen(path, "rb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    // get file size
    fseek(fileptr, 0, SEEK_END);
    file_size = ftell(fileptr);
    rewind(fileptr);

    // sprintf(cmd, "download %s", arg);

    // TODO: pass to max sequence
    sprintf(cmd, "%ld", file_size);

    size_t bufflen;
    fprintf(stdout, "Downloading: %s // Size: %ld // Num of packets: %ld\n", arg, file_size, file_size / MAX_DATA_SIZE);

    packet_t initial_packet = {
        .type = DATA,
        .seqn = 1,
        .max_seqn = 1,
        .data_length = sizeof(cmd),
        .data = cmd};
    Writen(sockfd, &initial_packet, 4 * sizeof(uint32_t));
    Writen(sockfd, initial_packet.data, initial_packet.data_length);

    do
    {
        bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE, fileptr);
        download_progress = (float)ftell(fileptr) / file_size;
        progress_bar(download_progress);

        // Send custom packet with characters read
        packet_t packet = {
            .type = DATA,
            .seqn = 1,
            .max_seqn = 1,
            .data_length = bufflen,
            .data = buffer};

        Writen(sockfd, &packet, 4 * sizeof(uint32_t));
        Writen(sockfd, packet.data, packet.data_length);
    } while (!feof(fileptr) && bufflen > 0);

    fclose(fileptr);
    free(buffer);
}

void cmd_delete(char const *arg, user_t const *user, int sockfd)
{
    char path[256];
    strcpy(path, user->dir);
    strncat(path, arg, strlen(arg) + 1);
    remove(path);
    propagate_delete(arg, user, sockfd);
}

void cmd_list_server(char const *arg, user_t const *user, int sockfd)
{
    char path[256];
    DIR *dir;
    struct dirent *dp;
    struct stat statbuf;
    struct tm *time;
    char mtime[256], atime[256], ctime[256];

    long int size;
    int total_files = 0;

    strncpy(path, "./", 3);
    strncat(path, user->dir, strlen(user->dir) + 1);

    dir = opendir(path);

    // get total files on dir
    while ((dp = readdir(dir)) != NULL)
    {
        // Ignore special dir
        if (strncmp(dp->d_name, ".", 1) == 0 || strncmp(dp->d_name, "..", 2) == 0)
            continue;

        total_files++;
    }
    // Restart dir reading
    rewinddir(dir);

    int data_size = snprintf(NULL, 0, "%s\t%s\t%s\t%s\t%s\n", "modification time (mtime)", "access time (atime)", "creation time (ctime)", "size", "name");
    char *data = (char *)malloc((data_size + 1) * sizeof(char));
    // Headers modification time (mtime), access time (atime) e change or creation time (ctime)
    snprintf(data, data_size + 1, "%s\t%s\t%s\t%s\t%s\n", "modification time (mtime)", "access time (atime)", "creation time (ctime)", "size", "name");

    packet_t packet = {
        .type = DATA,
        .seqn = 0,
        .max_seqn = total_files,
        .data_length = strlen(data) + 1,
        .data = data};

    Writen(sockfd, &packet, 4 * sizeof(uint32_t));
    Writen(sockfd, packet.data, packet.data_length);
    free(data);

    int files_read = 0;

    while ((dp = readdir(dir)) != NULL)
    {
        // Ignore special dir
        if (strncmp(dp->d_name, ".", 1) == 0 || strncmp(dp->d_name, "..", 2) == 0)
            continue;

        char *full_path = malloc(strlen(path) + strlen(dp->d_name) + 2); // +2 para a barra e o caractere nulo        strncat(full_path, path, strlen(path) + 1);
        strncpy(full_path, path, strlen(path) + 1);
        strncat(full_path, dp->d_name, strlen(dp->d_name) + 1);

        if (stat(full_path, &statbuf) == -1)
            continue;

        /* Time of last modification.  */
        time = localtime(&statbuf.st_mtime);
        strftime(mtime, sizeof(mtime), nl_langinfo(D_T_FMT), time);
        /* Time of last access.  */
        time = localtime(&statbuf.st_atime);
        strftime(atime, sizeof(atime), nl_langinfo(D_T_FMT), time);
        /* Time of last status change.  */
        time = localtime(&statbuf.st_ctime);
        strftime(ctime, sizeof(ctime), nl_langinfo(D_T_FMT), time);

        size = (intmax_t)statbuf.st_size;

        data_size = snprintf(NULL, 0, "%s\t%s\t%s\t%ld\t%s\n", mtime, atime, ctime, size, dp->d_name);
        char *data = (char *)malloc((data_size + 1) * sizeof(char));
        snprintf(data, data_size + 1, "%s\t%s\t%s\t%ld\t%s\n", mtime, atime, ctime, size, dp->d_name);

        packet_t packet = {
            .type = DATA,
            .seqn = files_read,
            .max_seqn = total_files,
            .data_length = strlen(data) + 1,
            .data = data};

        Writen(sockfd, &packet, 4 * sizeof(uint32_t));
        Writen(sockfd, packet.data, packet.data_length);

        free(data);
        free(full_path);
        files_read++;
    }
}

void cmd_exit(char const *arg, user_t const *user, int sockfd)
{
    printf("exit command\n");
    // TODO: fix
    return NULL;
}