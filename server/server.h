#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "simulation.h"
#include "../sockets-lib/socket.h"
#include "../pipe-lib/pipe.h"

#define BUFFER_SIZE 1024

typedef struct {
    int client_socket;
} client_data_t;

int run_server_internally(int port);
void initialize_server(int port);
void *handle_client(void *arg);
void run_server();
void process_client_input_socket(simulation_state_t *state, const char *input);
int run_server_internally_with_fifo(int requested_port, int fifo_fd);

#endif // SERVER_H
