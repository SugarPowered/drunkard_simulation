#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "menu.h"
#include "../sockets-lib/socket.h"

#define BUFFER_SIZE 1024

int main() {
    // Initialize the connection once
    int socket_fd = initialize_connection();
    if (socket_fd < 0) {
        // Handle error, exit, etc.
        return 1;
    }

    char buffer[BUFFER_SIZE];
    char user_input[BUFFER_SIZE];

    while (1) {
        // Example usage: send something
        // send_to_server("Hello server!");

        // Then receive something
        if (receive_from_server(socket_fd, buffer, sizeof(buffer)) > 0) {
            // Possibly display menu or do other logic
            display_menu();
        }

        // Check for 'exit'
        if (strcmp(user_input, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }
        sleep(1); // Delay for demonstration
    }

    // Explicitly close when we decide
    close_connection();
    return 0;
}
