#ifndef COMMAND_H
#define COMMAND_H

#include "user.h"
typedef struct cmd {
    char const* name;
    void (*func)(char const*, user_t const*, int);
    user_t const* user;
    int has_arg;
} cmd_t;

#define CMD(func, user, has_arg) {#func, cmd_ ## func, user, has_arg}

void parse_command(char* cmdline, user_t* user, int sockfd);
void send_command(int sockfd, char* str);
void send_device_auth(int sockfd);

void cmd_upload(char const* arg, user_t const* user, int sockfd);
void cmd_download(char const* arg, user_t const* user, int sockfd);
void cmd_delete(char const* arg, user_t const* user, int sockfd);
void cmd_list_server(char const* arg, user_t const* user, int sockfd);
void cmd_exit(char const* arg, user_t const* user, int sockfd);

#endif

