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

void cmd_download(char const *arg, char const *userdir, int sockfd)
{
    printf("download command with %s as argument\n", arg);

    // FILE *fileptr;
    // size_t file_size;
    // char *filename = strrchr(arg, '/');
    // char *buffer = (char *)malloc(MAX_DATA_SIZE * sizeof(char));
    // char cmd[MAXLINE];
    // float upload_progress = 0.0;

    // // Verify if file exists
    // if (!check_file_exists(arg))
    //     err_msg("file does not exists");

    // if (filename != NULL)
    // {
    //     filename++;
    // }

    // fileptr = fopen(arg, "rb");
    // if (fileptr == NULL)
    //     err_msg("failed to open file");

    // fseek(fileptr, 0, SEEK_END);
    // file_size = ftell(fileptr);
    // rewind(fileptr);

    // sprintf(cmd, "upload %s", filename);
    // send_command(sockfd, cmd);

    // sprintf(cmd, "%ld", file_size);
    // send_command(sockfd, cmd);

    // size_t bufflen;
    // fprintf(stdout, "Uploading: %s // Size: %ld // Num of packets: %ld\n", filename, file_size, file_size / MAX_DATA_SIZE);

    // do
    // {
    //     bufflen = fread(buffer, sizeof(char), MAX_DATA_SIZE, fileptr);
    //     upload_progress = (float)ftell(fileptr) / file_size;
    //     progress_bar(upload_progress);

    //     // Send custom packet with characters read
    //     packet_t packet = {
    //     .type = COMMAND,
    //     .seqn = 1,
    //     .max_seqn = 1,
    //     .data_length = bufflen,
    //     .data = buffer};

    //     Writen(sockfd, &packet, 4 * sizeof(uint32_t));
    //     Writen(sockfd, packet.data, packet.data_length);

    // } while (!feof(fileptr) && bufflen > 0);

    // free(buffer);
}

void cmd_delete(char const *arg, char const *userdir, int sockfd)
{
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

    char *data = (char*)Malloc((114) * sizeof(char));
    // Headers modification time (mtime), access time (atime) e change or creation time (ctime)
    sprintf(data, "%s\t%s\t\t%s\t%s\t%s\n", "modification time (mtime)", "access time (atime)", "creation time (ctime)", "size", "name");

    packet_t packet = {
        .type = DATA,
        .seqn = 0,
        .max_seqn = total_files,
        .data_length = strlen(data) + 1,
        .data = data};

    Writen(sockfd, &packet, 4 * sizeof(uint32_t));
    Writen(sockfd, packet.data, packet.data_length);
    free(data);

    for(int i = 0; i < total_files; i++){
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

        // 33 of \t and \0
        char *data = (char*)Malloc((strlen(mtime) + strlen(atime) + strlen(ctime) + strlen((char*)size)+ strlen(dp->d_name) + 33) * sizeof(char));

        sprintf("%s\t%s\t%s\t%jd\t%s\n", mtime, atime, ctime, size, dp->d_name);
        
        packet_t packet = {
        .type = DATA,
        .seqn = i,
        .max_seqn = total_files,
        .data_length = strlen(data),
        .data = data};

        Writen(sockfd, &packet, 4 * sizeof(uint32_t));
        Writen(sockfd, packet.data, packet.data_length);

        free(data);
        free(full_path);
    }
}

void cmd_exit(char const *arg, char const *userdir, int sockfd)
{
    printf("exit command\n");
}