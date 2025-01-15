#include "client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../sockets-lib/socket.h"
#include "../server/server.h"

// Make the socket global or static so it persists
static int client_socket = -1;

// Optional: Provide an initialization function to set up the socket
int initialize_connection() {
    if (client_socket != -1) {
        // Already initialized
        return client_socket;
    }

    // Connect to the server once and store the result
    client_socket = connect_to_server(SERVER_ADDRESS, SERVER_PORT);
    if (client_socket < 0) {
        fprintf(stderr, "Failed to connect to server.\n");
        return -1;
    }
    printf("Successfully connected to server on socket %d.\n", client_socket);
    return client_socket;
}

// Function to send data to the server without destroying the connection
void send_to_server(const char *message) {
    if (client_socket < 0) {
        fprintf(stderr, "Invalid socket. Did you call initialize_connection()?\n");
        return;
    }

    if (write(client_socket, message, strlen(message)) < 0) {
        perror("Error sending data to server");
        return;
    }

    printf("Message sent to server successfully.\n");
    // Notice we do NOT close the socket here
}

// If you still need a helper to receive messages:
int receive_from_server(int socket_fd, char *buffer, int buffer_size) {
    memset(buffer, 0, buffer_size);
    int bytes_received = read(socket_fd, buffer, buffer_size - 1);

    if (bytes_received < 0) {
        perror("Failed to receive message from server");
        return -1;
    }
    printf("Message received from server: %s\n", buffer);

    return bytes_received;
}

// Provide a cleanup function to be called explicitly when done
void close_connection() {
    if (client_socket >= 0) {
        printf("Closing connection on socket %d.\n", client_socket);
        active_socket_destroy(client_socket);
        client_socket = -1;
    }
}
