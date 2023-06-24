#ifndef PACKET_H
#define PACKET_H
#include <stdint.h>

#define COMMAND 1
#define DATA 2

typedef struct packet
{
    uint32_t type;
    uint32_t seqn;
    uint32_t max_seqn;
    uint32_t data_length;
    char *data;
} packet_t;

void send_packet(int sockfd, packet_t packet);

#endif