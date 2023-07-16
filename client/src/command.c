#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include "command.h"
#include "dir.h"
#include "error.h"
#include "packet.h"
#include "util.h"
#include "wrapper.h"
#include "util.h"

cmd_t dispatch_table[] = {
    CMD(upload, "sync_dir", 1),
    CMD(download, "sync_dir", 1),
    CMD(delete, "sync_dir", 1),
    CMD(list_server, "sync_dir", 0),
    CMD(list_client, "sync_dir", 0),
    CMD(exit, "sync_dir", 0),

    CMD(receive_upload, "sync_dir", 1),
    CMD(receive_delete, "sync_dir", 1)
    };

void progress_bar(float progress)
{
    int bar_width = 50;
    int filled = progress * bar_width;

    fprintf(stdout, "[");
    for (int i = 0; i < bar_width; i++)
    {
        if (i < filled)
            fprintf(stdout, "#");
        else
            fprintf(stdout, " ");
    }
    fprintf(stdout, "] %.2f%%\r", progress * 100.0);
    fflush(stdout);

    if (progress == 1.0)
        fprintf(stdout, "\n");
}

void parse_command(char *cmdline, const char *userdir, int sockfd)
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
            char const *first_arg = strtok_r(NULL, delim, &saveptr);
            char const *second_arg = strtok_r(NULL, delim, &saveptr);

            if (current_cmd.has_arg && first_arg == NULL)
                err_msg("Too few arguments to command %s", current_cmd.name);
            else if (current_cmd.has_arg && second_arg != NULL)
                err_msg("Too many arguments to command %s", current_cmd.name);
            else if (!current_cmd.has_arg && first_arg != NULL)
                err_msg("Too many arguments to command %s", current_cmd.name);
            else
                current_cmd.func(sockfd, userdir, first_arg);

            return;
        }
    }

    err_msg("Unrecognized command %s\n", cmd);
}

char *read_command()
{
    size_t len = MAXLINE;
    char *buff = Malloc(len);
    Getline(&buff, &len, stdin);

    return buff;
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

int recv_device_auth(int sockfd)
{
    char buff[1];
    return read(sockfd, buff, sizeof(char));
}

void cmd_upload(int sockfd, char const *userdir, char const *arg)
{
    FILE *fileptr;
    size_t file_size;
    char *filename = strrchr(arg, '/');
    char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
    char cmd[MAXLINE];
    float upload_progress = 0.0;
    // Current packet and max packets
    int seq = 1;
    int max_seq;

    // Verify if file exists
    if (!check_file_exists(arg))
        err_msg("file does not exists");

    if (filename != NULL)
    {
        filename++;
    }

    fileptr = fopen(arg, "rb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    // get file size
    fseek(fileptr, 0, SEEK_END);
    file_size = ftell(fileptr);
    rewind(fileptr);

    // TODO: VER cmd_list_client
    sprintf(cmd, "upload %s ctime", filename);
    send_command(sockfd, cmd);

    // Total packets to send
    max_seq = file_size / MAX_DATA_SIZE;

    size_t bufflen;
    fprintf(stdout, "Uploading: %s // Size: %ld // Num of packets: %ld\n", filename, file_size, file_size / MAX_DATA_SIZE);

    do
    {
        bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE, fileptr);
        upload_progress = (float)ftell(fileptr) / file_size;
        progress_bar(upload_progress);

        // Send custom packet with characters read
        packet_t packet = {
            .type = DATA,
            .seqn = seq,
            .max_seqn = max_seq,
            .data_length = bufflen,
            .data = buffer};

        Writen(sockfd, &packet, 4 * sizeof(uint32_t));
        Writen(sockfd, packet.data, packet.data_length);
        seq++;
    } while (!feof(fileptr) && bufflen > 0);

    free(buffer);
}

void cmd_download(int sockfd, char const *userdir, char const *arg)
{
    char buff[MAXLINE];
    char path[256];
    sprintf(buff, "download %s", arg);
    send_command(sockfd, buff);

    packet_t packet = recv_packet(sockfd);

    long file_size = strtol(packet.data, NULL, 10);
    strcpy(path, userdir);
    strncat(path, arg, strlen(arg) + 1);
    FILE *fileptr = fopen(path, "wb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    while (file_size > 0)
    {
        packet = recv_packet(sockfd);
        fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
        file_size -= packet.data_length;
    }

    fclose(fileptr);
    fprintf(stdout, "File download complete: %s\n", arg);
}

void cmd_delete(int sockfd, char const *userdir, char const *arg)
{
    char buff[MAXLINE];
    sprintf(buff, "delete %s", arg);
    send_command(sockfd, buff);
}

void cmd_list_server(int sockfd, char const *userdir, char const *arg)
{
    char *cmd = "list_server";
    send_command(sockfd, cmd);

    // Headers
    packet_t packet = recv_packet(sockfd);
    fprintf(stdout, "%s\n", packet.data);

    // wait for response
    while (packet.seqn <= packet.max_seqn)
    {
        packet = recv_packet(sockfd);
        fprintf(stdout, "%s", packet.data);
    } 
}

void cmd_list_client(int sockfd, char const *userdir, char const *arg)
{
    char path[256];
    DIR *dir;
    struct dirent *dp;
    struct stat statbuf;
    struct tm *time;
    char mtime[256], atime[256], ctime[256];
    long int size;

    strncpy(path, "./", 3);
    strncat(path, userdir, strlen(userdir) + 1);
    dir = opendir(path);

    // Headers modification time (mtime), access time (atime) e change or creation time (ctime)
    printf("%s\t%s\t\t%s\t%s\t%s\n", "modification time (mtime)", "access time (atime)", "creation time (ctime)", "size", "name");

    while ((dp = readdir(dir)) != NULL)
    {
        // Ignore special dir
        if (strncmp(dp->d_name, ".", 1) == 0 || strncmp(dp->d_name, "..", 2) == 0)
            continue;

        char *full_path = malloc(strlen(path) + strlen(dp->d_name) + 2); // +2 para a barra e o caractere nulo        strncat(full_path, path, strlen(path) + 1);
        strncpy(full_path, path, strlen(path) + 1);
        strncat(full_path, dp->d_name, strlen(dp->d_name) + 1);

        if (stat(full_path, &statbuf) == -1)
            continue;

        /* Time of last modification.  */
        time = localtime(&statbuf.st_mtime);
        strftime(mtime, sizeof(mtime), nl_langinfo(D_T_FMT), time);
        /* Time of last access.  */
        time = localtime(&statbuf.st_mtime);
        strftime(atime, sizeof(atime), nl_langinfo(D_T_FMT), time);
        /* Time of last status change.  */
        time = localtime(&statbuf.st_mtime);
        strftime(ctime, sizeof(ctime), nl_langinfo(D_T_FMT), time);

        size = (intmax_t)statbuf.st_size;

        // send packet
        printf("%s\t%s\t%s\t%jd\t%s\n", mtime, atime, ctime, size, dp->d_name);
        free(full_path);
    }
}

void cmd_receive_upload(int sockfd, char const* dir, char const* arg)
{
    char path[265]; // 256 + sync_dir_
    strcpy(path, dir);
    strncat(path, arg, sizeof(path) - strlen(path) - 1);

    FILE *fileptr = fopen(path, "wb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    packet_t packet = {
        .type = DATA,
        .seqn = 0,
        .max_seqn = 1,
        .data_length = 0,
        .data = ""};

    while (packet.seqn <= packet.max_seqn)
    {
        packet = recv_packet(sockfd);
        fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
    }

    fclose(fileptr);
}

void cmd_receive_delete(int sockfd, char const* dir, char const* arg)
{
    char path[256];
    strcpy(path, dir);
    strncat(path, arg, strlen(arg) + 1);
    remove(path);
}

void cmd_exit(int sockfd, char const *userdir, char const *arg)
{
    char *cmd = "exit";
    send_command(sockfd, cmd);
    Close(sockfd);
    exit(EXIT_SUCCESS);
}