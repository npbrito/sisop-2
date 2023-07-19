#ifndef COMMAND_H
#define COMMAND_H

#include "user.h"

typedef struct cmd {
    char const* name;
    void (*func)(char const*, int, user_t*, int);
    int has_arg;
    user_t* user;
    int device_id;
} cmd_t;

#define CMD(func, has_arg, user ,device_id) {#func, cmd_ ## func, has_arg, user ,device_id}

void parse_command(char* cmdline, int sockfd, user_t* user, int device_id);
void send_command(int sockfd, char* str);
void send_device_auth(int sockfd);

void cmd_upload(char const* arg, int sockfd, user_t* user, int device_id);
void cmd_download(char const* arg, int sockfd, user_t* user, int device_id);
void cmd_delete(char const* arg, int sockfd, user_t* user, int device_id);
void cmd_list_server(char const* arg, int sockfd, user_t* user, int device_id);
void cmd_exit(char const* arg, int sockfd, user_t* user, int device_id);

#endif

