#include "packet.h"
#include "util.h"

void send_packet(int sockfd, packet_t packet)
{
    Writen(sockfd, &packet, 4 * sizeof(uint32_t));
    Writen(sockfd, packet.data, packet.data_length);
}