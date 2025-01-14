#pragma once

#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "menu.h"
#include "../sockets-lib/socket.h"

#define BUFFER_SIZE 1024

int main() {
    // Pripojenie k serveru
    int socket_fd = connect_to_server(SERVER_ADDRESS, SERVER_PORT);

    char buffer[BUFFER_SIZE];
    char user_input[BUFFER_SIZE];

    printf("Connected to the server. \n");

    while (1) {
        // Prijatie správy od servera
        if (receive_from_server(socket_fd, buffer, sizeof(buffer)) > 0) {
            display_menu();
        }

        // Získanie vstupu od používateľa
        printf("You: ");
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
            fprintf(stderr, "Error reading input.\n");
            break;
        }

        // Odstránenie nového riadku z používateľského vstupu
        size_t len = strlen(user_input);
        if (user_input[len - 1] == '\n') {
            user_input[len - 1] = '\0';
        }

        // Ak používateľ zadá "exit", ukončíme program
        if (strcmp(user_input, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        // Odoslanie používateľského vstupu na server
        send_to_server(user_input);

        // Cakanie na odozvu od servera, vykreslenie simulacie v terminali
        if (receive_from_server(socket_fd, buffer, sizeof(buffer)) > 0) {
            printf("Server: %s\n", buffer);
            run_renderer(socket_fd);
        }
    }

    return 0;
}
