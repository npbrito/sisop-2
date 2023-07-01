#include <string.h>
#include <stdio.h>
#include "command.h"
#include "packet.h"
#include "wrapper.h"
#include "error.h"

cmd_t dispatch_table[] = {
    CMD(upload, 1),
    CMD(download, 1),
    CMD(delete, 1),
    CMD(list_server, 0),
    CMD(exit, 0)
};

void parse_command(char* cmdline, int sockfd)
{
    char const* delim = " \n";
    char* saveptr;
    char const* cmd = strtok_r(cmdline, delim, &saveptr);
    size_t num_cmds = sizeof dispatch_table / sizeof dispatch_table[0];

    for (int i = 0; i < num_cmds; i++) {
        cmd_t current_cmd = dispatch_table[i];

        if (!strcmp(cmd, current_cmd.name)) {
            char const* arg = strtok_r(NULL, delim, &saveptr);
            current_cmd.func(arg, sockfd);
            return;
        }
    }
}

void send_command(int sockfd, char* str)
{
    packet_t packet = {
        .type = COMMAND,  
        .seqn = 1,
        .max_seqn = 1,
        .data_length = strlen(str) + 1,  // str + '\0',
        .data = str
    };

    send_packet(sockfd, packet);
}

void send_device_auth(int sockfd)
{
    Write(sockfd, "1", sizeof(char));
}

void cmd_upload(char const* arg, int sockfd)
{
    printf("upload command with %s as argument\n", arg);
}

void cmd_download(char const* arg, int sockfd)
{
    printf("download command with %s as argument\n", arg);
}

void cmd_delete(char const* arg, int sockfd)
{
    printf("delete command with %s as argument\n", arg);
}

void cmd_list_server(char const* arg, int sockfd)
{
    printf("list_server command\n");
}

void cmd_exit(char const* arg, int sockfd)
{
    printf("exit command\n");
}