#ifndef COMMAND_H
#define COMMAND_H

#include "user.h"
typedef struct cmd {
    char const* name;
    void (*func)(char const*, char const*, int);
    char const* dir;
    int has_arg;
} cmd_t;

#define CMD(func, dir, has_arg) {#func, cmd_ ## func, dir, has_arg}

void parse_command(char* cmdline, char* userdir, int sockfd);
void send_command(int sockfd, char* str);
void send_device_auth(int sockfd);

void cmd_upload(char const* arg, char const* dir, int sockfd);
void cmd_download(char const* arg, char const* dir, int sockfd);
void cmd_delete(char const* arg, char const* dir, int sockfd);
void cmd_list_server(char const* arg, char const* dir, int sockfd);
void cmd_exit(char const* arg, char const* dir, int sockfd);

#endif

