#ifndef COMMAND_H
#define COMMAND_H

typedef struct cmd {
    char const* name;
    void (*func)(int, char const*, char const*);
    char const* dir;
    int has_arg;
} cmd_t;


#define CMD(func, dir, has_arg) {#func, cmd_ ## func, dir, has_arg}

void parse_command(char* cmdline, const char *userdir, int sockfd);
char* read_command();
void send_command(int sockfd, char* str);
int recv_device_auth(int sockfd);

void cmd_upload(int sockfd, char const *userdir, char const* arg);
void cmd_download(int sockfd, char const *userdir, char const* arg);
void cmd_delete(int sockfd, char const *userdir, char const* arg);
void cmd_list_server(int sockfd, char const* arg);
void cmd_list_client(int sockfd, char const* arg);
void cmd_exit(int sockfd, char const* arg);

#endif

