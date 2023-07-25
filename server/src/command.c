#include <string.h>
#include <stdio.h>
#include "command.h"
#include "packet.h"
#include "wrapper.h"
#include "error.h"
#include "sockutil.h"

cmd_t dispatch_table[] = {
    CMD(upload, 1, NULL, 1),
    CMD(download, 1, NULL, 1),
    CMD(delete, 1, NULL, 1),
    CMD(list_server, 0, NULL, 1),
    CMD(exit, 0, NULL, 1)};

void parse_command(char *cmdline, int sockfd, user_t *user, int device_id)
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
            current_cmd.func(arg, sockfd, user, device_id);
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

void cmd_upload(char const *arg, int sockfd, user_t *user, int device_id)
{
    char path[265]; // 256 + sync_dir_
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    strcpy(path, user->dir);
    strcat(path, "/");
    strncat(path, arg, sizeof(path) - strlen(path) - 1);
    printf("Filepath: %s\n", path);
    pthread_mutex_lock(&m);
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
        printf("File: %s\n", fileptr);
        printf("Filename: %s\n", user->dir);
        fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
    }

    fclose(fileptr);
    pthread_mutex_unlock(&m);
    fprintf(stdout, "File upload complete: %s\n", arg);
    propagate_upload(arg, sockfd, user, device_id);
}

void propagate_upload(char const *arg, int sockfd, user_t *user, int device_id)
{
    int i, device_count;

    client_t *client = get_client_by_user(user);
    device_count = get_device_count(&(client->devices));

    if (client!=NULL){
        for(i = 0; i < device_count; i++){
            if(i != device_id) // enviamos para todos menos oque enviou
            {
                int servsock = client->devices[i]->servconn;
                send_file(arg, servsock, user, device_id);
            }
        }
    }
}

void send_file(char const *arg, int sockfd, user_t *user, int device_id))
{
     {
        FILE *fileptr;
        size_t file_size;
        char path[256];

        strcpy(path, user->dir);
        strcat(path, "/");
        strncat(path, arg, strlen(arg) + 1);

        char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
        char cmd[MAXLINE];
        float download_progress = 0.0;
        pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

        // Verify if file exists
        if (!check_file_exists(path))
            err_msg("file does not exist");

        pthread_mutex_lock(&m);
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

        // tamanho do arquivo
        packet_t initial_packet = {
            .type = DATA,
            .seqn = 1,
            .max_seqn = 1,
            .data_length = sizeof(cmd),
            .data = cmd};
        Writen(sockfd, &initial_packet, 4 * sizeof(uint32_t));
        Writen(sockfd, initial_packet.data, initial_packet.data_length);

        // nome do arquivo
        packet_t packet_filename = {
            .type = DATA,
            .seqn = 1,
            .max_seqn = 1,
            .data_length = sizeof(arg),
            .data = arg};
        }
        Writen(sockfd, &packet_filename, 4 * sizeof(uint32_t));
        Writen(sockfd, packet_filename.data, packet_filename.data_length);

        do
        {
            bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE, fileptr);

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
        pthread_mutex_unlock(&m);
        free(buffer);
    }
}

int get_file_size(FILE* ptrfile){
    int size;

	fseek(ptrfile, 0L, SEEK_END);
	size = ftell(ptrfile);

	rewind(ptrfile);

	return size;
}

void cmd_download(char const *arg, int sockfd, user_t *user, int device_id)
{
    {
        FILE *fileptr;
        size_t file_size;
        char path[256];

        strcpy(path, user->dir);
        strcat(path, "/");
        strncat(path, arg, strlen(arg) + 1);
        printf("FilePath: %s\n", path);
        char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
        char cmd[MAXLINE];
        float download_progress = 0.0;
        pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

        // Verify if file exists
        if (!check_file_exists(path))
            err_msg("file does not exist");

        pthread_mutex_lock(&m);
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
            // progress_bar(download_progress);

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
        pthread_mutex_unlock(&m);
        free(buffer);
    }
}

void cmd_delete(char const *arg, int sockfd, user_t *user, int device_id)
{
    char path[256];
    strcpy(path, user->dir);
    strcat(path, "/");
    printf("FilePath: %s\n", path);
    strncat(path, arg, strlen(arg) + 1);
    remove(path);
    // propagate_delete(arg, user, sockfd);
}

void cmd_list_server(char const *arg, int sockfd, user_t *user, int device_id)
{
    printf("list_server command\n");
}

void cmd_exit(char const *arg, int sockfd, user_t *user, int device_id)
{
    printf("exit command\n");
    client_t *clients = get_clients_list();
    client_t *client = get_client_by_user(clients, user->username);

    device_t *devices = &(client->devices);
    remove_device(devices, device_id);
    // Destruir thread
}