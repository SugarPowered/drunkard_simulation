#ifndef CLIENT_H
#define CLIENT_H

#include "../common/protocol.h"
#include "../sockets-lib/socket.h"
#include "renderer.h"

#define SERVER_PORT 13141
#define SERVER_ADDRESS "127.0.0.1"

int send_message(int socket, message_type_t type, const char *data);
int receive_message(int socket, protocol_message_t *msg);
void send_to_server(const char *message);
int receive_from_server(int socket, char *buffer, int buffer_size);
int initialize_connection();
void close_connection();

#endif // CLIENT_H
