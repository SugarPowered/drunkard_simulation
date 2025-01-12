#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "../sockets-lib/socket.h"

void process_client_input(simulation_state_t *state, const char *input) {
    if (strncmp(input, "START_SIMULATION", 16) == 0) {
        printf("Starting a new simulation...\n");
        // Extract and process parameters for the simulation
    } else if (strncmp(input, "REPLAY_SIMULATION", 17) == 0) {
        printf("Replaying a previous simulation...\n");
        // Load and handle the replay logic
    } else {
        printf("Unknown command: %s\n", input);
    }
}

void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_socket = client_data->client_socket;
    free(client_data);

    char buffer[BUFFER_SIZE];
    const char *welcome_msg = "Welcome to the Random Walk Simulation Server!\n";
    write(client_socket, welcome_msg, strlen(welcome_msg));

    simulation_state_t *state = get_simulation_state(); // Get simulation state

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1);

        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received from client: %s\n", buffer);

        // Process client input
        process_client_input(state, buffer);

        const char *response = "Command processed.\n";
        write(client_socket, response, strlen(response));
    }

    close(client_socket);
    return NULL;
}

void run_server(int server_socket) {

    printf("Server is running on port %d. Waiting for connections...\n", PORT);

    while (1) {
        int client_socket = passive_socket_wait_for_client(server_socket);

        if (client_socket < 0) {
            fprintf(stderr, "Failed to accept client connection.\n");
            continue;
        }

        printf("Client connected!\n");

        // Allocate memory for client data and spawn a thread
        client_data_t *client_data = malloc(sizeof(client_data_t));
        if (!client_data) {
            fprintf(stderr, "Failed to allocate memory for client data.\n");
            close(client_socket);
            continue;
        }

        client_data->client_socket = client_socket;
        pthread_t client_thread;

        if (pthread_create(&client_thread, NULL, handle_client, client_data) != 0) {
            fprintf(stderr, "Failed to create thread for client.\n");
            free(client_data);
            close(client_socket);
            continue;
        }

        // Detach the thread so resources are released automatically when it finishes
        pthread_detach(client_thread);
    }

    passive_socket_destroy(server_socket);
}

void initialize_server(int port) {
    int server_socket = passive_socket_init(port);
    if (server_socket < 0) {
        fprintf(stderr, "Failed to initialize server socket on port %d.\n", port);
        exit(EXIT_FAILURE);
    }

    printf("Server initialized on port %d.\n", port);
    run_server(server_socket);
}
