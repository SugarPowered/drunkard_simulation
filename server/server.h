#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "simulation.h"

#include "../sockets-lib/socket.h"

#define PORT 13141
#define BUFFER_SIZE 2048

typedef struct {
    int client_socket;
} client_data_t;

void initialize_server(int port);
void *handle_client(void *arg);
void run_server();
void process_client_input_socket(const char *input);

#endif // SERVER_H
