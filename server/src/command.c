#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include "command.h"
#include "packet.h"
#include "wrapper.h"
#include "error.h"
#include "sockutil.h"
#include "util.h"
#include "math.h"

cmd_t dispatch_table[] = {
    CMD(upload, "sync_dir", 1),
    CMD(download, "sync_dir", 1),
    CMD(delete, "sync_dir", 1),
    CMD(list_server, "sync_dir", 0),
    CMD(exit, "sync_dir", 0)};

void parse_command(char *cmdline, char *userdir, int sockfd)
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
            char const *arg = strtok_r(NULL, delim, &saveptr);
            current_cmd.func(arg, userdir, sockfd);
            return;
        }
    }
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

void send_device_auth(int sockfd)
{
    Write(sockfd, "1", sizeof(char));
}

void cmd_upload(char const *arg, char const *userdir, int sockfd)
{
    printf("upload command with %s as argument\n", arg);

    char path[265]; // 256 + sync_dir_
    strcpy(path, userdir);
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
    fprintf(stdout, "File upload complete: %s\n", arg);
}

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

void cmd_download(char const *arg, char const *dir, int sockfd)
{
    printf("download command with %s as argument\n", arg);

    FILE *fileptr;
    size_t file_size;
    char path[256];
    // char *filename = strrchr(arg, '/');
    // printf("Filename: %s\n", filename);
    strcpy(path, dir);
    strncat(path, arg, strlen(arg) + 1);
    char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
    char cmd[MAXLINE];
    float download_progress = 0.0;

    // if (filename != NULL)
    // {
    //     filename++;
    // }

    // Verify if file exists
    if (!check_file_exists(path))
        err_msg("file does not exist");

    fileptr = fopen(path, "rb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    // get file size
    fseek(fileptr, 0, SEEK_END);
    file_size = ftell(fileptr);
    rewind(fileptr);

    // sprintf(cmd, "download %s", arg);

    // TODO: pass to max sequence
    sprintf(cmd, "%ld", file_size);

    size_t bufflen;
    fprintf(stdout, "Downloading: %s // Size: %ld // Num of packets: %ld\n", arg, file_size, file_size / MAX_DATA_SIZE);

    packet_t initial_packet = {
        .type = DATA,
        .seqn = 1,
        .max_seqn = 1,
        .data_length = sizeof(cmd),
        .data = cmd};
    Writen(sockfd, &initial_packet, 4 * sizeof(uint32_t));
    Writen(sockfd, initial_packet.data, initial_packet.data_length);

    do
    {
        bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE, fileptr);
        download_progress = (float)ftell(fileptr) / file_size;
        progress_bar(download_progress);

        // Send custom packet with characters read
        packet_t packet = {
            .type = DATA,
            .seqn = 1,
            .max_seqn = 1,
            .data_length = bufflen,
            .data = buffer};

        Writen(sockfd, &packet, 4 * sizeof(uint32_t));
        Writen(sockfd, packet.data, packet.data_length);
    } while (!feof(fileptr) && bufflen > 0);

    fclose(fileptr);
    free(buffer);
}

void cmd_delete(char const *arg, char const *userdir, int sockfd)
{
    // TODO Delete the file itself
    remove(arg);
    printf("delete command with %s as argument\n", arg);
}

void cmd_list_server(char const *arg, char const *userdir, int sockfd)
{
    printf("list_server command on %s\n", userdir);

    char path[256];
    DIR *dir;
    struct dirent *dp;
    struct stat statbuf;
    struct tm *time;
    char mtime[256], atime[256], ctime[256];

    long int size;
    int total_files = 0;

    strncpy(path, "./", 3);
    strncat(path, userdir, strlen(userdir) + 1);

    dir = opendir(path);

    // get total files on dir
    while ((dp = readdir(dir)) != NULL)
    {
        // Ignore special dir
        if (strncmp(dp->d_name, ".", 1) == 0 || strncmp(dp->d_name, "..", 2) == 0)
            continue;

        total_files++;
    }
    // Restart dir reading
    rewinddir(dir);

    int data_size = snprintf(NULL, 0, "%s\t%s\t%s\t%s\t%s\n", "modification time (mtime)", "access time (atime)", "creation time (ctime)", "size", "name");
    char *data = (char*)malloc((data_size + 1) * sizeof(char));
    // Headers modification time (mtime), access time (atime) e change or creation time (ctime)
    snprintf(data, data_size + 1, "%s\t%s\t%s\t%s\t%s\n", "modification time (mtime)", "access time (atime)", "creation time (ctime)", "size", "name");

    packet_t packet = {
        .type = DATA,
        .seqn = 0,
        .max_seqn = total_files,
        .data_length = strlen(data) + 1,
        .data = data};

    Writen(sockfd, &packet, 4 * sizeof(uint32_t));
    Writen(sockfd, packet.data, packet.data_length);
    free(data);

    int files_read = 0;

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

        data_size = snprintf(NULL, 0, "%s\t%s\t%s\t%ld\t%s\n", mtime, atime, ctime, size, dp->d_name);
        char *data = (char*)malloc((data_size + 1) * sizeof(char));
        snprintf(data, data_size + 1, "%s\t%s\t%s\t%ld\t%s\n", mtime, atime, ctime, size, dp->d_name);

        packet_t packet = {
            .type = DATA,
            .seqn = files_read,
            .max_seqn = total_files,
            .data_length = strlen(data) + 1,
            .data = data};

        Writen(sockfd, &packet, 4 * sizeof(uint32_t));
        Writen(sockfd, packet.data, packet.data_length);

        free(data);
        free(full_path);
        files_read++;
    }
}

void cmd_exit(char const *arg, char const *userdir, int sockfd)
{
    printf("exit command\n");
}