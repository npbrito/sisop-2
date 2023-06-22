#ifndef COMMAND_H
#define COMMAND_H

char* readcmd();
void sendcmd(int sockfd, char* str);

#endif

