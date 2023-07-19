#include <string.h>
#include <stdio.h>
#include "command.h"
#include "packet.h"
#include "wrapper.h"
#include "error.h"
#include "sockutil.h"

cmd_t dispatch_table[] = {
    CMD(upload, 1, NULL ,1),
    CMD(download, 1, NULL ,1),
    CMD(delete, 1, NULL ,1),
    CMD(list_server, 0, NULL ,1),
    CMD(exit, 0, NULL, 1)
};

void parse_command(char* cmdline, int sockfd, user_t* user, int device_id)
{
    char const* delim = " \n";
    char* saveptr;
    char const* cmd = strtok_r(cmdline, delim, &saveptr);
    size_t num_cmds = sizeof dispatch_table / sizeof dispatch_table[0];

    for (int i = 0; i < num_cmds; i++) {
        cmd_t current_cmd = dispatch_table[i];

        if (!strcmp(cmd, current_cmd.name)) {
            char const* arg = strtok_r(NULL, delim, &saveptr);
            current_cmd.func(arg, sockfd, user, device_id);
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

void cmd_upload(char const* arg, int sockfd, user_t* user, int device_id)
{
    printf("upload command with %s as argument\n", arg);
}

void cmd_download(char const* arg, int sockfd, user_t* user, int device_id)
{
    printf("download command with %s as argument\n", arg);
}

void cmd_delete(char const* arg, int sockfd, user_t* user, int device_id)
{
    printf("delete command with %s as argument\n", arg);
}

void cmd_list_server(char const* arg, int sockfd, user_t* user, int device_id)
{
    printf("list_server command\n");
}

void cmd_exit(char const* arg, int sockfd, user_t* user, int device_id)
{
    printf("exit command\n");
    // TODO: pegatr client da main - global var
    client_t *clients = get_clients_list();
    printf("clients \n");
    client_t *client = get_client_by_user(clients, user->username);
    
    printf("-- client get_client_by_user \n");
    device_t *devices = &(client->devices);
    printf("--- device_t \n");

    remove_device(devices, device_id);
    // Send OK to client
    // Destruir thread
}