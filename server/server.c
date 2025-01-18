#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "../common/protocol.h"
#include "../sockets-lib/socket.h"

int client_sockets[MAX_CLIENTS] = {0};
int num_clients = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void cleanup_client(int client_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = 0;
            if (i == num_clients - 1) num_clients--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    close(client_socket);
}

void cleanup_server() {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] > 0) {
            close(client_sockets[i]);
            client_sockets[i] = 0;
        }
    }
    num_clients = 0;
    pthread_mutex_unlock(&clients_mutex);
    pthread_mutex_destroy(&clients_mutex);
}

int send_to_client(int socket, message_type_t type, const char *data) {
    protocol_message_t msg;
    msg.type = type;
    msg.length = strlen(data);
    strncpy(msg.data, data, MSG_MAX_SIZE - 1);

    return write(socket, &msg, sizeof(protocol_message_t));
}

int receive_from_client(int socket, protocol_message_t *msg) {
    int bytes = read(socket, msg, sizeof(protocol_message_t));
    if (bytes <= 0) return bytes;

    msg->data[msg->length] = '\0';
    return bytes;
}

void process_client_input(const char *input) {
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

    send_to_client(client_socket, MSG_WELCOME, "Vita vas simulacia nahodnej pochodzky.\n");

    simulation_state_t *state = get_simulation_state();
    protocol_message_t msg;

    while (1) {
        int bytes_received = receive_from_client(client_socket, &msg);
        if (bytes_received <= 0) {
            printf("Klient odpojeny.\n");
            break;
        }

        switch (msg.type) {
            case MSG_START_SIM:
                printf("Startuje sa nova simulacia...\n");
                process_client_input_locally(msg.data);

                char file_content[BUFF_DATA_SIZE] = {0};
                FILE *file = fopen(state->results_file, "r");
                if (file) {
                    size_t bytes_read = fread(file_content, sizeof(char), BUFF_DATA_SIZE - 1, file);
                    file_content[bytes_read] = '\0';
                    fclose(file);
                    send_to_client(client_socket, MSG_SIM_COMPLETE, file_content);
                } else {
                    send_to_client(client_socket, MSG_ERROR, "Nepodarilo sa nacitat vysledky simulacie.");
                }
                break;

            case MSG_REPLAY:
                printf("Opatovne spustenie predoslej simulacie...\n");
                reset_simulation();
                break;

            case MSG_DISCONNECT:
                printf("Klient poziadal o odpojenie.\n");
                goto cleanup;

            default:
                printf("Neznamy prikaz.\n");
                send_to_client(client_socket, MSG_ERROR, "Neznamy prikaz.");
                break;
        }
    }

	cleanup:
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
