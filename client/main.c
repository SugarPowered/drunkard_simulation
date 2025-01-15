#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "menu.h"
#include "../sockets-lib/socket.h"

#define BUFFER_SIZE 1024

int main() {
    // 1. Initialize the connection once
    int socket_fd = initialize_connection();
    if (socket_fd < 0) {
        fprintf(stderr, "Error: Could not connect to server.\n");
        return 1;
    }

    // Possibly send or wait to receive data from the server
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));

        // 2. Wait for data from server
        int bytes_received = receive_from_server(socket_fd, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            printf("Server disconnected or error reading.\n");
            break;
        }

        if (strncmp(buffer, "SIMULATION_COMPLETED:", 21) == 0) {
            // If server signals an exit
            printf("SERVER SAID: SIMULATION COMPLETED! YAY\n");
            break;

        } else {
            // Unknown or unhandled header
            printf("Unhandled message from server: %s\n", buffer);
        }

        // 4. Possibly prompt user or do menu logic
        display_menu();

        // Example: read user input, check for "exit", send to server, etc.
        // ...
    }

    // 5. Clean up and close connection
    close_connection();
    return 0;
}
