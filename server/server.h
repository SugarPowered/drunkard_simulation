#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../sockets-lib/socket.h"
#include "simulation.h"

#define PORT 13141
#define BUFFER_SIZE 2048
#define MAX_CLIENTS 10

extern int client_sockets[MAX_CLIENTS];
extern int num_clients;
extern pthread_mutex_t clients_mutex;

typedef struct {
    int client_socket;
} client_data_t;

void cleanup_client(int client_socket);
void cleanup_server();
int send_to_client(int socket, message_type_t type, const char *data);
void initialize_server(int port);
void *handle_client(void *arg);
void run_server();
void process_client_input_socket(const char *input);

#endif // SERVER_H
