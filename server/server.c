// server.c
#include "server.h"
#include "../sockets-lib/socket.h"
// Structure to pass client socket to thread

// Function to handle a client connection

void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_socket = client_data->client_socket;
    free(client_data);

    char buffer[BUFFER_SIZE];
    const char *welcome_msg = "Welcome to the Random Walk Simulation Server!\n";
    write(client_socket, welcome_msg, strlen(welcome_msg));

    simulation_state_t *state = get_simulation_state(); // Získanie stavu zo simulácie

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1);

        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received from client: %s\n", buffer);

        // Spracovanie vstupu klienta pomocou externého volania
        process_client_input(state, buffer);

        const char *response = "Command processed.\n";
        write(client_socket, response, strlen(response));
    }

    close(client_socket);
    return NULL;
}

// Main server loop
void run_server() {
    int server_socket = passive_socket_init(PORT);

    if (server_socket < 0) {
        fprintf(stderr, "Failed to initialize server socket.\n");
        exit(EXIT_FAILURE);
    }

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

// server.c
void initialize_server(int port) {
    int server_socket = passive_socket_init(port);
    if (server_socket < 0) {
        fprintf(stderr, "Failed to initialize server socket on port %d.\n", port);
        exit(EXIT_FAILURE);
    }

    printf("Server initialized on port %d.\n", port);
    run_server();
}

