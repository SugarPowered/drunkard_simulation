//
// Created by michal.subert on 10. 1. 2025.
//

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h> // funguje na WIN
//#include <arpa/inet.h> funguje iba na UNIX
// todo can import instead ==> /home/shared/POS/sockets-lib
#include <pthread.h>
#include "simulation.h"



#define PORT 8080

typedef struct {
    int socket;
    struct sockaddr_in address;
} Server;

void server_start(Server *server);
void *handle_client(void *arg);

#endif // SERVER_H
