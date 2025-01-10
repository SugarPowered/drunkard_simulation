//
// Created by michal.subert on 10. 1. 2025.
//

#include "server.h"

void server_start(Server *server) {

    // todo needs re-design, can use /home/shared/POS/sockets-lib

    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(PORT);

    if (bind(server->socket, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        perror("Bind failed");
        close(server->socket);
        exit(EXIT_FAILURE);
    }

    listen(server->socket, 3);
    printf("Server started. Waiting for clients...\n");

    while (1) {
        int client_socket = accept(server->socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Client accept failed");
            continue;
        }
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)(intptr_t)client_socket);
        pthread_detach(thread_id); // Detach thread to handle independently
    }
}

void *handle_client(void *arg) {
    int client_socket = (intptr_t)arg;


    // todo Handle client requests (e.g., starting simulation, sending updates)

    close(client_socket);
    return NULL;
}