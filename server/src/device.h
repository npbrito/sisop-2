#ifndef DEVICE_H
#define DEVICE_H

#include "user.h"
#include "sockutil.h"

typedef struct device {
    uint32_t id;
    conndata_t cmdconn;
    conndata_t cliconn;
    conndata_t servconn;
    device_t* next;
} device_t;

void add_device(device_t* head, int id, conndata_t cmdconn);
device_t* get_device_by_id(device_t* head, int id);
int recv_device_id(int sockfd);

#endif