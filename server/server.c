#pragma once

#include "server.h"

void send_menu_state(int client_socket) {
    const char *message = "INFO|MENU";

    ssize_t bytes_written = write(client_socket, message, strlen(message));
    if (bytes_written < 0) {
        perror("Failed to send menu state to client");
    } else {
        printf("Menu state notification sent to client.\n");
    }
}

static int pick_random_port(int min_port, int max_port) {
    return min_port + rand() % (max_port - min_port + 1);
}

static int create_server_socket(int requested_port, int *final_port_out) {
    if (!final_port_out) {
        fprintf(stderr, "create_server_socket called with NULL final_port_out!\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned)time(NULL));

    int server_socket = -1;
    int attempts = 100;

    if (requested_port != 0) {
        server_socket = passive_socket_init(requested_port);
        if (server_socket < 0) {
            fprintf(stderr, "Failed to bind to port %d.\n", requested_port);
            exit(EXIT_FAILURE);
        }
        *final_port_out = requested_port;
        printf("create_server_socket: Bound to fixed port %d\n", requested_port);
    } else {
        const int min_port = 49152;
        const int max_port = 65535;
        while (attempts--) {
            int rport = pick_random_port(min_port, max_port);
            server_socket = passive_socket_init(rport);
            if (server_socket >= 0) {
                *final_port_out = rport;
                printf("create_server_socket: Bound to random port %d\n", rport);
                break;
            }
        }
        if (server_socket < 0) {
            fprintf(stderr, "Failed to find free random port after many attempts.\n");
            exit(EXIT_FAILURE);
        }
    }

    return server_socket;
}

int run_server_internally(int requested_port) {
    simulation_state_t *state = get_simulation_state();

    int final_port = 0;
    int server_socket = create_server_socket(requested_port, &final_port);

    initialize_simulation(server_socket);

    run_server(server_socket);

    return 0;
}

int run_server_internally_with_fifo(int requested_port, int fifo_fd) {
    simulation_state_t *state = get_simulation_state();

    int final_port = 0;
    int server_socket = create_server_socket(requested_port, &final_port);

    if (write(fifo_fd, &final_port, sizeof(final_port)) == -1) {
        fprintf(stderr, "ERROR: could not write ephemeral port to FIFO.\n");
    }
    close(fifo_fd);

    initialize_simulation(server_socket);

    run_server(server_socket);

    return 0;
}


void initialize_server(int requested_port) {
    int final_port = 0;
    int server_socket = create_server_socket(requested_port, &final_port);

    printf("initialize_server: Actually bound on port %d\n", final_port);

    run_server(server_socket);
}

void process_client_input(const char *input, int client_socket) {
    if (strncmp(input, "START_SIMULATION", 16) == 0) {
        printf("Startuje sa nova simulacia...\n");
        process_client_input_locally(input, client_socket);
    } else if (strncmp(input, "REPLAY_SIMULATION", 17) == 0) {
        printf("Opatovne spustenie predoslej simulacie...\n");
        reset_simulation(client_socket);
    } else {
        printf("Neznamy prikaz: %s\n", input);
    }
}

void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_socket = client_data->client_socket;
    free(client_data);

    char buffer[BUFFER_SIZE];
    const char *welcome_msg = "INFO|MENU";
    write(client_socket, welcome_msg, strlen(welcome_msg));

    simulation_state_t *state = get_simulation_state();

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1);

        if (bytes_received <= 0) {
            printf("Klient odpojeny.\n");
            break;
        }

        printf("[CLIENT->SERVER] %s\n", buffer);
        process_client_input(buffer, client_socket);

        char file_content[BUFF_DATA_SIZE] = {0};
        FILE *file = fopen(global_simulation_state.results_file, "r");
        if (file) {
            size_t bytes_read = fread(file_content, sizeof(char), BUFF_DATA_SIZE - 1, file);
            file_content[bytes_read] = '\0';
            fclose(file);
        }

		break;
    }

 	close(client_socket);
    return NULL;
}

void run_server(int server_socket) {
    printf("Server is running. Waiting for connections...\n");

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
