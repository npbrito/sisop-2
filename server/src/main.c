#include <stdbool.h>
#include <stdlib.h>
#include "error.h"
#include "util.h"
#include "wrapinet.h"
#include "wrappthread.h"
#include "wrapsock.h"
#include "wrapstdio.h"
#include "wrapunix.h"
#include "tcputil.h"
#include "sockutil.h"
#include "packet.h"
#include "user.h"
#include "command.h"
#include "data.h"

static void *handler(void *arg);

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

    client_t *clients = NULL;

    while (true)
    {
        conndata_t *conndata = accept_connection(listenfd);
        connlist_t connlist = {
            .clients = clients,
            .conndata = conndata};
        handle_connection(&connlist, &handler);
    }

    return EXIT_SUCCESS;
}

static void *handler(void *arg)
{
    connlist_t connlist = *(connlist_t *)arg;
    free(arg);
    Pthread_detach(pthread_self());
    conndata_t conndata = *(connlist.conndata);
    client_t *clients = connlist.clients;

    print_client(conndata.cliaddr);
    user_t user = recv_user(conndata.connfd);
    setup_user(user);
    uint32_t device_id = recv_id(conndata.connfd);
    uint32_t conn_id = recv_id(conndata.connfd);

    switch (conn_id)
    {
    case 1:
        client_t *client = get_client_by_user(clients, user.username);
        if (client == NULL)
        {
            device_t device = {
                .id = device_id,
                .cmdconn = conndata,
                .next = NULL};
            add_client(clients, user, device);
        }
        else
        {
        }

        break;
    case 2:
        break;
    case 3:
        break;
    default:
        err_quit("Invalid connection ID.");
    }

    while (true)
    {
        packet_t packet = recv_packet(conndata.connfd);

        if (packet.type == COMMAND)
            parse_command(packet.data, conndata.connfd);
        else
            parse_data(packet, conndata.connfd);

        free(packet.data);
    }

    Close(conndata.connfd);

    return NULL;
}