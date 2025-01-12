#ifndef CLIENT_H
#define CLIENT_H

#include "../sockets-lib/socket.h"

// Definícia portu a adresy servera
#define SERVER_PORT 27199
#define SERVER_ADDRESS "127.0.0.1"

// Funkcie na komunikáciu so serverom
void send_to_server(int socket, const char *message);
int receive_from_server(int socket, char *buffer, int buffer_size);

#endif // CLIENT_H
