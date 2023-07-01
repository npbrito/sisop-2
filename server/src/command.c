#include <string.h>
#include <stdio.h>
#include "command.h"
#include "packet.h"
#include "wrapunix.h"
#include "error.h"

cmd_t dispatch_table[] = {
    CMD(upload, 1),
    CMD(download, 1),
    CMD(delete, 1),
    CMD(list_server, 0),
    CMD(get_sync_dir, 0),
    CMD(exit, 0)};

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
            char const *arg = strtok_r(NULL, delim, &saveptr);
            current_cmd.func(arg, sockfd);
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

void cmd_upload(char const *arg, int sockfd)
{
    printf("upload command with %s as argument\n", arg);

    packet_t packet = recv_packet(sockfd);

    printf("file_size in char %s \n", packet.data);

    // TODO: fix this
    long file_size = strtol(packet.data, NULL, 10);
    printf("file_size converted to long %ld \n", file_size);

    FILE *fileptr = fopen(arg, "wb");

    if (fileptr == NULL)
        err_msg("failed to open file");
    
    while (file_size > 0)
    {
        packet = recv_packet(sockfd);
        fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
        
        file_size -= packet.data_length;
        printf("file_size - packet.data_length = %ld \n", file_size);
    } 

    fclose(fileptr);
    fprintf(stdout, "File upload complete: %s\n", arg);
}

void cmd_download(char const *arg, int sockfd)
{
    printf("download command with %s as argument\n", arg);
}

void cmd_delete(char const *arg, int sockfd)
{
    printf("delete command with %s as argument\n", arg);
}

void cmd_list_server(char const *arg, int sockfd)
{
    printf("list_server command\n");
}

void cmd_get_sync_dir(char const *arg, int sockfd)
{
    printf("get_sync_dir command\n");
}

void cmd_exit(char const *arg, int sockfd)
{
    printf("exit command\n");
}