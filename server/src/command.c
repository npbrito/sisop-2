#include <string.h>
#include <stdio.h>
#include "command.h"
#include "packet.h"
#include "wrapper.h"
#include "error.h"

cmd_t dispatch_table[] = {
    CMD(upload, 1),
    CMD(download, 1),
    CMD(delete, 1),
    CMD(list_server, 0),
    CMD(exit, 0)
};

void parse_command(char *cmdline, int sockfd)
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
            current_cmd.func(arg, sockfd);
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

void cmd_upload(char const* arg, int sockfd)
{
    printf("upload command with %s as argument\n", arg);

    packet_t packet = recv_packet(sockfd);

    long file_size = strtol(packet.data, NULL, 10);
    
    FILE *fileptr = fopen(arg, "wb");
    if (fileptr == NULL)
        err_msg("failed to open file");

    while (file_size > 0)
    {
        packet = recv_packet(sockfd);
        fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
        file_size -= packet.data_length;
    } 

    fclose(fileptr);
    fprintf(stdout, "File upload complete: %s\n", arg);
}

void cmd_download(char const *arg, int sockfd)
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

void cmd_delete(char const *arg, int sockfd)
{
    printf("delete command with %s as argument\n", arg);
}

void cmd_list_server(char const *arg, int sockfd)
{
    printf("list_server command\n");
}

void cmd_exit(char const *arg, int sockfd)
{
    printf("exit command\n");
}