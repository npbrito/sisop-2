#include <string.h>
#include <unistd.h>
#include "command.h"
#include "error.h"
#include "packet.h"
#include "wrapper.h"
#include "util.h"

cmd_t dispatch_table[] = {
    CMD(upload, 1),
    CMD(download, 1),
    CMD(delete, 1),
    CMD(list_server, 0),
    CMD(list_client, 0),
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
            char const* first_arg = strtok_r(NULL, delim, &saveptr);
            char const* second_arg = strtok_r(NULL, delim, &saveptr);

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

    err_msg("Unrecognized command %s", cmd);
}

char* read_command()
{
    size_t len = MAXLINE;
    char* buff = Malloc(len);
    Getline(&buff, &len, stdin);

    return buff;
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

int recv_device_auth(int sockfd)
{
    char buff[1];
    return read(sockfd, buff, sizeof(char));
}

void cmd_upload(int sockfd, char const* arg)
{
    char buff[MAXLINE];
    sprintf(buff, "upload %s", arg);
    send_command(sockfd, buff);
    printf("upload command with %s as argument\n", arg);
}

void cmd_download(int sockfd, char const* arg)
{
    char buff[MAXLINE];
    sprintf(buff, "download %s", arg);
    send_command(sockfd, buff);
    printf("download command with %s as argument\n", arg);
}

void cmd_delete(int sockfd, char const* arg)
{
    char buff[MAXLINE];
    sprintf(buff, "delete %s", arg);
    send_command(sockfd, buff);
    printf("delete command with %s as argument\n", arg);
}

void cmd_list_server(int sockfd, char const* arg)
{
    char* cmd = "list_server";
    send_command(sockfd, cmd);
    printf("list_server command\n");
}

void cmd_list_client(int sockfd, char const* arg)
{
    printf("list_client command\n");
}

void cmd_exit(int sockfd, char const* arg)
{
    char* cmd = "exit";
    send_command(sockfd, cmd);
    // Close(sockfd);
    // exit(EXIT_SUCCESS);
}