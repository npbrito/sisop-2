#include <string.h>
#include <unistd.h>
#include "command.h"
#include "error.h"
#include "packet.h"
#include "wrapper.h"
#include "util.h"

cmd_t dispatch_table[] = {
    CMD(upload, "sync_dir", 1),
    CMD(download, "sync_dir", 1),
    CMD(delete, "sync_dir", 1),
    CMD(list_server, "sync_dir", 0),
    CMD(list_client, "sync_dir", 0),
    CMD(exit, "sync_dir", 0)};

void parse_command(char *cmdline, const char *userdir, int sockfd)
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
            char const *first_arg = strtok_r(NULL, delim, &saveptr);
            char const *second_arg = strtok_r(NULL, delim, &saveptr);

            if (current_cmd.has_arg && first_arg == NULL)
                err_msg("Too few arguments to command %s", current_cmd.name);
            else if (current_cmd.has_arg && second_arg != NULL)
                err_msg("Too many arguments to command %s", current_cmd.name);
            else if (!current_cmd.has_arg && first_arg != NULL)
                err_msg("Too many arguments to command %s", current_cmd.name);
            else
                current_cmd.func(sockfd, userdir, first_arg);

            return;
        }
    }

    err_msg("Unrecognized command %s", cmd);
}

char *read_command()
{
    size_t len = MAXLINE;
    char *buff = Malloc(len);
    Getline(&buff, &len, stdin);

    return buff;
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

int recv_device_auth(int sockfd)
{
    char buff[1];
    return read(sockfd, buff, sizeof(char));
}

void cmd_upload(int sockfd, char const *userdir, char const *arg)
{
    FILE *fileptr;
    size_t file_size;
    char *filename = strrchr(arg, '/');
    char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
    char cmd[MAXLINE];
    // float upload_progress = 0.0;
    //  Current packet and max packets
    int seq = 1;
    int max_seq;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    // Verify if file exists
    if (!check_file_exists(arg))
    {

        err_msg("file does not exists");
    }
    else
    {

        if (filename != NULL)
        {
            filename++;
        }
        Pthread_mutex_lock(&mutex);
        fileptr = fopen(arg, "rb");
        if (fileptr == NULL)
            err_msg("failed to open file");

        // get file size
        fseek(fileptr, 0, SEEK_END);
        file_size = ftell(fileptr);
        rewind(fileptr);

        sprintf(cmd, "upload %s ctime", filename);
        send_command(sockfd, cmd);

        // Total packets to send
        max_seq = file_size / MAX_DATA_SIZE;

        size_t bufflen;
        fprintf(stdout, "Uploading: %s // Size: %ld // Num of packets: %ld\n", filename, file_size, file_size / MAX_DATA_SIZE);

        do
        {
            bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE, fileptr);
            // upload_progress = (float)ftell(fileptr) / file_size;
            // progress_bar(upload_progress);

            // Send custom packet with characters read
            packet_t packet = {
                .type = DATA,
                .seqn = seq,
                .max_seqn = max_seq,
                .data_length = bufflen,
                .data = buffer};

            Writen(sockfd, &packet, 4 * sizeof(uint32_t));
            Writen(sockfd, packet.data, packet.data_length);
            seq++;
        } while (!feof(fileptr) && bufflen > 0);

        fclose(fileptr);
        Pthread_mutex_unlock(&mutex);
        free(buffer);
    }
}

void cmd_download(int sockfd, char const *userdir, char const *arg)
{
    char buff[MAXLINE];
    char path[256];
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    sprintf(buff, "download %s", arg);
    send_command(sockfd, buff);

    packet_t packet = recv_packet(sockfd);

    long file_size = strtol(packet.data, NULL, 10);
    strcpy(path, userdir);
    printf("FilePath: %s\n", path);
    strcat(path, "/");
    printf("FilePath: %s\n", path);
    strncat(path, arg, strlen(arg) + 1);
    printf("FilePath: %s\n", path);
    Pthread_mutex_lock(&m);
    FILE *fileptr = fopen(path, "wb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    while (file_size > 0)
    {
        packet = recv_packet(sockfd);
        fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
        file_size -= packet.data_length;
    }

    fclose(fileptr);
    Pthread_mutex_unlock(&m);
    fprintf(stdout, "File download complete: %s\n", arg);
}

void cmd_delete(int sockfd, char const *userdir, char const *arg)
{
    char buff[MAXLINE];
    sprintf(buff, "delete %s", arg);
    send_command(sockfd, buff);
}

void cmd_list_server(int sockfd, char const *arg)
{
    char *cmd = "list_server";
    send_command(sockfd, cmd);
    printf("list_server command\n");
}

void cmd_list_client(int sockfd, char const *arg)
{
    printf("list_client command\n");
}

void cmd_exit(int sockfd, char const *arg)
{
    char *cmd = "exit";
    send_command(sockfd, cmd);
    // Close(sockfd);
    // exit(EXIT_SUCCESS);
}