#ifndef COMMAND_H
#define COMMAND_H

char* recv_command(int sockfd);
void send_command(int sockfd, char* str);

#endif

