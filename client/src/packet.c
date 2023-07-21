#include "packet.h"
#include "util.h"

void send_packet(int sockfd, packet_t packet)
{
    Writen(sockfd, &packet, 4 * sizeof(uint32_t));
    Writen(sockfd, packet.data, packet.data_length);
}

packet_t recv_packet(int sockfd)
{
    packet_t packet;
    Readn(sockfd, &packet, 4 * sizeof(uint32_t));
    packet.data = Malloc(packet.data_length);
    Readn(sockfd, packet.data, packet.data_length);
    return packet;
}