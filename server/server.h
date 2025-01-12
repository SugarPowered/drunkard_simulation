//
// Created by michal.subert on 10. 1. 2025.
//

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../sockets-lib/socket.h"
#include "simulation.h"

#define PORT 12345
#define BUFFER_SIZE 1024



typedef struct {
    int client_socket;
} client_data_t;

// server.h
void initialize_server(int port);
void *handle_client(void *arg);
void run_server();

#endif // SERVER_H
