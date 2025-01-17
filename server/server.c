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

        char file_content[BUFF_DATA_SIZE] = {0};
        FILE *file = fopen(state->results_file, "r");
        if (file) {
            fread(file_content, sizeof(char), BUFF_DATA_SIZE, file);
            fclose(file);
        }

        char response[1024 + BUFF_DATA_SIZE];
		snprintf(response, sizeof(response), "SIMULATION_COMPLETED:\n %s", file_content);
//		printf("Chystam sa dorucit klientovi: %s\n", response);
        int check = write(client_socket, response, strlen(response));
        printf("Check poslanych bytov: %d\n", check);
        if (check > 21) {
          break;
        }
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

void initialize_server(int port) {
    int server_socket = passive_socket_init(port);
    if (server_socket < 0) {
        fprintf(stderr, "Nepodarilo sa inicializovat server socket na porte %d.\n", port);
        exit(EXIT_FAILURE);
    }

    printf("Server inicializovany na porte: %d.\n", port);
    run_server(server_socket);
}
