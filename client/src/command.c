#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "command.h"
#include "dir.h"
#include "error.h"
#include "packet.h"
#include "util.h"
#include "wrapper.h"

cmd_t dispatch_table[] = {
    CMD(upload, 1),
    CMD(download, 1),
    CMD(delete, 1),
    CMD(list_server, 0),
    CMD(list_client, 0),
    CMD(exit, 0)};

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

void parse_command(char *cmdline, int sockfd)
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
                current_cmd.func(sockfd, first_arg);

            return;
        }
    }

    err_msg("Unrecognized command %s\n", cmd);
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

// TODO: send current user and device
void cmd_upload(int sockfd, char const *arg)
{
    FILE *fileptr;
    size_t file_size;
    char *filename = strrchr(arg, '/');
    char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
    char cmd[MAXLINE];
    float upload_progress = 0.0;


    // Verify if file exists
    if (!check_file_exists(arg))
        err_msg("file does not exists");


    if (filename != NULL)
    {
        filename++;
    }

    fileptr = fopen(arg, "rb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    fseek(fileptr, 0, SEEK_END);
    file_size = ftell(fileptr);
    rewind(fileptr);

    sprintf(cmd, "upload %s", filename);
    send_command(sockfd, cmd);

    // Send num of packets
    sprintf(cmd, "%ld", file_size / MAX_DATA_SIZE);
    send_command(sockfd, cmd);
    

    size_t bufflen;
    fprintf(stdout, "Uploading: %s // Size: %ld // Num of packets: %ld\n", filename, file_size, file_size / MAX_DATA_SIZE);

    do
    {   
        bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE, fileptr);       
        upload_progress = (float)ftell(fileptr) / file_size;
        progress_bar(upload_progress);

        // Send custom packet with characters read
        packet_t packet = {
        .type = COMMAND,
        .seqn = 1,
        .max_seqn = 1,
        .data_length = bufflen,
        .data = buffer};

        Writen(sockfd, &packet, 4 * sizeof(uint32_t));
        Writen(sockfd, packet.data, packet.data_length);

    } while (!feof(fileptr) && bufflen > 0);

    free(buffer);
}

void cmd_download(int sockfd, char const *arg)
{
    char buff[MAXLINE];
    sprintf(buff, "download %s", arg);
    send_command(sockfd, buff);
    printf("download command with %s as argument\n", arg);
}

void cmd_delete(int sockfd, char const *arg)
{
    char buff[MAXLINE];
    sprintf(buff, "delete %s", arg);
    send_command(sockfd, buff);
    printf("delete command with %s as argument\n", arg);
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
    Close(sockfd);
    exit(EXIT_SUCCESS);
}