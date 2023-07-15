#ifndef SOCKUTIL_H
#define SOCKUTIL_H
#include <netinet/in.h>
#include <stdint.h>
#include "packet.h"
#include <errno.h>

#include "user.h"

typedef struct conndata
{
    struct sockaddr_in cliaddr;
    int connfd;
} conndata_t;

typedef struct device
{
    uint32_t id;
    conndata_t cmdconn;
    conndata_t fsconn;
    conndata_t servconn;
    struct device *next;
} device_t;

typedef struct client
{
    user_t user;
    device_t devices;
    struct client *next;
} client_t;

extern client_t *clients;

void add_device(device_t *head, int id, conndata_t cmdconn);
device_t *get_device_by_id(device_t *head, int id);
uint32_t recv_id(int sockfd);
void add_client(client_t **head, user_t user, device_t device);
conndata_t *accept_connection(int listenfd);
client_t *get_client_by_user(client_t *clients, char const *username);
void handle_connection(conndata_t *conndata, void *(*handler)(void *));
void print_client(struct sockaddr_in cliaddr);
void print_server(int sockfd);
uint32_t str_to_int(const char *str);
int get_device_count(device_t *head);

#endif