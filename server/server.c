#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "../sockets-lib/socket.h"

void process_client_input(simulation_state_t *state, const char *input) {
    if (strncmp(input, "START_SIMULATION", 16) == 0) {
        printf("Startuje sa nova simulacia...\n");
        process_client_input_locally(input);
    } else if (strncmp(input, "REPLAY_SIMULATION", 17) == 0) {
        printf("Opatovne spustenie predoslej simulacie...\n");
        reset_simulation();
    } else {
        printf("Neznamy prikaz: %s\n", input);
    }
}

void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_socket = client_data->client_socket;
    free(client_data);

    char buffer[BUFFER_SIZE];
    const char *welcome_msg = "Vita vas simulacia nahodnej pochodzky.\n";
    write(client_socket, welcome_msg, strlen(welcome_msg));

    simulation_state_t *state = get_simulation_state();

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1);

        if (bytes_received <= 0) {
            printf("Klient odpojeny.\n");
            break;
        }

        printf("Dorucene od klienta: %s\n", buffer);

        process_client_input(state, buffer);

        const char response[1024];
		snprintf(response, sizeof(response), "SIMULATION_COMPLETED:\n %s", file_buff);
		printf("Chystam sa dorucit klientovi: %s\n", response);
        int check = write(client_socket, response, strlen(response));
        printf("Check poslanych bytov: %d\n", check);
    }

    close(client_socket);
    return NULL;
}

void run_server(int server_socket) {

    printf("Server bezi na porte %d. Cakam na pripojenie...\n", PORT);

    while (1) {
        int client_socket = passive_socket_wait_for_client(server_socket);

        if (client_socket < 0) {
            fprintf(stderr, "Nepodarilo sa prijat pripojenie od klienta.\n");
            continue;
        }

        printf("Klient pripojeny!\n");

        client_data_t *client_data = malloc(sizeof(client_data_t));
        if (!client_data) {
            fprintf(stderr, "Nepodarilo sa alokovat pamat pre klientove data.\n");
            close(client_socket);
            continue;
        }

        client_data->client_socket = client_socket;
        pthread_t client_thread;

        if (pthread_create(&client_thread, NULL, handle_client, client_data) != 0) {
            fprintf(stderr, "Nepodarilo sa vytvorit vlakno pre klienta.\n");
            free(client_data);
            close(client_socket);
            continue;
        }

        pthread_detach(client_thread);
    }

    passive_socket_destroy(server_socket);
}

static int pick_random_port(int min_port, int max_port) {
    // Simple random port from [min_port, max_port]
    return min_port + rand() % (max_port - min_port + 1);
}

void initialize_server(int requested_port) {
    // If requested_port == 0, we will pick a random free port from e.g. 49152..65535
    // (the standard ephemeral range)
    // You could also just rely on the OS ephemeral port by letting port=0 in the bind,
    // but this snippet does it manually.
    int port_to_try = requested_port;
    if (port_to_try == 0) {
        // Example ephemeral range:
        int min_port = 49152;
        int max_port = 65535;

        // Try up to e.g. 100 random attempts:
        int attempts = 100;
        int server_socket;
        while (attempts--) {
            port_to_try = pick_random_port(min_port, max_port);
            server_socket = passive_socket_init(port_to_try);
            if (server_socket >= 0) {
                // Success binding
                printf("Server initialized on random port: %d\n", port_to_try);
                run_server(server_socket);
                return;
            }
        }
        fprintf(stderr, "Failed to find a free port after 100 attempts.\n");
        exit(EXIT_FAILURE);
    } else {
        // The user specifically requested a port
        int server_socket = passive_socket_init(port_to_try);
        if (server_socket < 0) {
            fprintf(stderr, "Failed to initialize server socket on port %d.\n", port_to_try);
            exit(EXIT_FAILURE);
        }
        printf("Server initialized on port: %d\n", port_to_try);
        run_server(server_socket);
    }
}
