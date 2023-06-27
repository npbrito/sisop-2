#include <stdlib.h>
#include "wrapunix.h"
#include "client.h"

void add_client(client_t* head, user_t user, device_t device)
{
    if (head == NULL) {
        head = Malloc(sizeof(client_t));
        head->user = user;
        head->devices = device;
        head->next->next = NULL;
    } else {
        client_t* current = head;

        while (current->next != NULL)
            current = current-> next;

        current->next = Malloc(sizeof(client_t));
        current->next->user = user;
        current->next->devices = device;
        current->next->next = NULL;
    }
}

// void add_client(client_t* head, user_t user, device_t device);