#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "sockutil.h"
#include "wrapinet.h"
#include "wrapsock.h"
#include "wrappthread.h"
#include "wrapunix.h"

void print_server(int listenfd)
{
	struct sockaddr_in servaddr;
	socklen_t len = sizeof servaddr;
	Getsockname(listenfd, (SA *)&servaddr, &len);
	uint16_t port = ntohs(servaddr.sin_port);

	// Uses UDP Socket to get local network address
	int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in cliaddr;
	memset(&cliaddr, 0, sizeof cliaddr);
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = 0;
	Inet_pton(AF_INET, "8.8.8.8", &cliaddr.sin_addr);
	Connect(sockfd, (SA *)&cliaddr, sizeof cliaddr);
	len = sizeof cliaddr;
	Getsockname(sockfd, (SA *)&cliaddr, &len);

	char buff[MAXLINE];
	printf("Server listening on IP: %s PORT: %hu\n",
		   Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof buff),
		   port);
}

void print_client(struct sockaddr_in cliaddr)
{
	char buff[MAXLINE];
	printf("Client connecting from IP: %s PORT: %hu\n",
		   Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof buff),
		   ntohs(cliaddr.sin_port));
}

conndata_t *accept_connection(int listenfd)
{
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof cliaddr;
	int connfd = Accept(listenfd, (SA *)&cliaddr, &len);
	conndata_t *conndata = Malloc(sizeof(conndata_t));
	conndata->cliaddr = cliaddr;
	conndata->connfd = connfd;

	return conndata;
}

void handle_connection(connlist_t *connlist, void *(*handler)(void *))
{
	pthread_t tid;
	Pthread_create(&tid, NULL, handler, connlist);
}

void add_client(client_t *head, user_t user, device_t device)
{
	if (head == NULL)
	{
		head = Malloc(sizeof(client_t));
		head->user = user;
		head->devices = device;
		head->next->next = NULL;
	}
	else
	{
		client_t *current = head;

		while (current->next != NULL)
			current = current->next;

		current->next = Malloc(sizeof(client_t));
		current->next->user = user;
		current->next->devices = device;
		current->next->next = NULL;
	}
}

void add_device(device_t *head, int id, conndata_t cmdconn)
{
	device_t *current = head;

	while (current->next != NULL)
		current = current->next;

	current->next = Malloc(sizeof(device_t));
	current->next->id = id;
	current->next->cmdconn = cmdconn;
	current->next->next = NULL;
}

device_t *get_device_by_id(device_t *head, int id)
{
	device_t *current = head;

	while (current != NULL)
	{
		if (current->id == id)
			return current;

		current = current->next;
	}

	return NULL;
}

uint32_t recv_id(int sockfd)
{
	packet_t packet = recv_packet(sockfd);
	size_t len = strlen(packet.data) + 1; // + 1 because of '\0'
	uint32_t id = str_to_int(packet.data);
	free(packet.data);

	return id;
}

uint32_t str_to_int(const char *str)
{
	char *end;
	errno = 0;
	long val = strtol(str, &end, 10);

	if (errno != 0 || val > UINT32_MAX || val < 0 || str == end || '\0' != *end)
		err_quit("invalid device id error");

	return (uint32_t)val;
}

client_t *get_client_by_user(client_t *head, char const *username)
{
	client_t *current = head;

	while (current != NULL)
	{
		if (!strcmp(username, current->user.username))
			return current;
		current = current->next;
	}

	return NULL;
}