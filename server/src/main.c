#include <stdbool.h>
#include <stdlib.h>
#include "error.h"
#include "util.h"
#include "wrapper.h"
#include "tcputil.h"
#include "sockutil.h"
#include "packet.h"
#include "user.h"
#include "command.h"
#include "data.h"
#include <unistd.h>
#include <stdio.h>

static void *handler(void *arg);

client_t *clients = NULL;

int main(int argc, char *argv[argc + 1])
{
    int listenfd;

    if (argc == 1) // Use any free port
        listenfd = Tcp_listen(NULL);
    else if (argc == 2)
        listenfd = Tcp_listen(argv[1]);
    else
        err_quit("usage: ./myServer [ <service/port#> ]\n");

    print_server(listenfd);

    while (true)
    {
        conndata_t *conndata = accept_connection(listenfd);
        handle_connection(conndata, &handler);
    }

    return EXIT_SUCCESS;
}

static void *handler(void *arg)
{
    conndata_t conndata = *(conndata_t *)arg;
    free(arg);
    Pthread_detach(pthread_self());
    
    print_client(conndata.cliaddr);
    user_t user = recv_user(conndata.connfd);
    uint32_t device_id = recv_id(conndata.connfd);
    uint32_t clithread_id = recv_id(conndata.connfd);
    client_t *client = get_client_by_user(clients, user.username);
    device_t* device;

    switch (clithread_id) {
        case 1: // Command line thread
            if (client == NULL)
            {
                device_t device = {
                    .id = device_id,
                    .cmdconn = conndata,
                    .next = NULL
                };

                add_client(&clients, user, device);
            } else {
                int device_count = get_device_count(&(client->devices));
                
                if (device_count > 1) 
                    goto cleanup;
                else {
                    add_device(&(client->devices), device_id, conndata);
                }
            }
            get_sync_dir(user);
            send_device_auth(conndata.connfd);
            break;
        case 2: // File system listen thread
            device = get_device_by_id(&(client->devices), device_id);
            device->fsconn = conndata;
            break;
        case 3: // Server listen thread
            device = get_device_by_id(&(client->devices), device_id);
            device->servconn = conndata;
            // TODO Think about this!
            break;
        default:
            err_quit("Invalid connection ID.");
    }

    while (true)
    {
        packet_t packet = recv_packet(conndata.connfd);
        parse_command(packet.data, conndata.connfd);
        free(packet.data);
    }

cleanup:
    free(user.username);
    free(user.dir);
    Close(conndata.connfd);

    return NULL;
}