#ifndef COMMAND_H
#define COMMAND_H

char* read_command();
void send_command(int sockfd, char* str);

#endif

