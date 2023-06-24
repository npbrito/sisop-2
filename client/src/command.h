#ifndef COMMAND_H
#define COMMAND_H

typedef struct cmd {
    char const* name;
    void (*func)(int, char const*);
    int has_arg;
} cmd_t;

#define CMD(func, has_arg) {#func, cmd_ ## func, has_arg}

void parse_command(char* cmdline, int sockfd);
char* read_command();
void send_command(int sockfd, char* str);

void cmd_upload(int sockfd, char const* arg);
void cmd_download(int sockfd, char const* arg);
void cmd_delete(int sockfd, char const* arg);
void cmd_list_server(int sockfd, char const* arg);
void cmd_list_client(int sockfd, char const* arg);
void cmd_exit(int sockfd, char const* arg);

#endif

