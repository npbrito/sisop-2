#include <stdio.h>
#include <string.h>
#include "device.h"
#include "packet.h"
#include <errno.h>

void add_device(device_t* head, int id, conndata_t cmdconn)
{
    device_t* current = head;

    while (current->next != NULL)
        current = current-> next;

    current->next = Malloc(sizeof(device_t));
    current->next->id = id;
    current->next->cmdconn = cmdconn;
    current->next->next = NULL;
}

device_t* get_device_by_id(device_t* head, int id)
{
    device_t* current = head;

    while (current != NULL) {
        if (current->id == id)
            return current;

        current = current->next;
    }

    return NULL;
}

int recv_device_id(int sockfd)
{
    packet_t packet = recv_packet(sockfd);
    size_t len = strlen(packet.data) + 1;   // + 1 because of '\0'
    int device_id = str_to_int(packet.data);
    free(packet.data);

    return device_id;
}

uint32_t str_to_int(const char* str)
{
    char* end;
    errno = 0;
    long val = strtol(str, &end, 10);

    if (errno != 0 || val > UINT32_MAX || val < 0 || str == end || '\0' != *end)
        err_quit("invalid device id error");

    return (uint32_t) val;
}