#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../sockets-lib/socket.h"
#include "simulation.h"

#define PORT 27200
#define BUFFER_SIZE 1024

typedef struct {
    int client_socket;
} client_data_t;

// server.h
void initialize_server(int port);
void *handle_client(void *arg);
void run_server();
void process_client_input_socket(simulation_state_t *state, const char *input);

#endif // SERVER_H
