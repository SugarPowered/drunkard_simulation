#ifndef CLIENT_H
#define CLIENT_H

#include "../sockets-lib/socket.h"
#include "renderer.h"

extern int client_socket;

void send_to_server(const char *message);
int receive_from_server(int socket, char *buffer, int buffer_size);
int initialize_connection();
void close_connection();

#endif // CLIENT_H
