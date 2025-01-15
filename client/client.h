#ifndef CLIENT_H
#define CLIENT_H

#include "../sockets-lib/socket.h"
#include "renderer.h"

#define SERVER_PORT 13141
#define SERVER_ADDRESS "127.0.0.1"

int initialize_connection();
void send_to_server(const char *message);
int receive_from_server(int socket, char *buffer, int buffer_size);
void close_connection();

#endif // CLIENT_H
