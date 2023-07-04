#ifndef COMMAND_H
#define COMMAND_H

typedef struct cmd {
    char const* name;
    void (*func)(char const*, int);
    int has_arg;
} cmd_t;

#define CMD(func, has_arg) {#func, cmd_ ## func, has_arg}

void parse_command(char* cmdline, int sockfd);
void send_command(int sockfd, char* str);
void send_device_auth(int sockfd);

void cmd_upload(char const* arg, int sockfd);
void cmd_download(char const* arg, int sockfd);
void cmd_delete(char const* arg, int sockfd);
void cmd_list_server(char const* arg, int sockfd);
void cmd_exit(char const* arg, int sockfd);

#endif

