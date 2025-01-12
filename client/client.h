//
// Created by michal.subert on 10. 1. 2025.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "renderer.h"
#include "input_handler.h"

#define PORT 8080

typedef struct {
    int socket;
} Client;

void client_connect(Client *client, const char *ip, int port);
void receive_data(Client *client);

#endif // CLIENT_H
