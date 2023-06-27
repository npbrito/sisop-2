#ifndef CLIENT_H
#define CLIENT_H

#include "user.h"
#include "device.h"

typedef struct client {
    user_t user;
    device_t devices;
    client_t* next;
} client_t;

void add_client(client_t* head, user_t user, device_t device);

#endif