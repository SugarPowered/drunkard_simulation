#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "renderer.h"
#include "../sockets-lib/socket.h"

#define WORLD_SIZE 20
#define BUFFER_SIZE 1024

char world[WORLD_SIZE][WORLD_SIZE];

// Function to clear the world array
void clear_world() {
    for (int i = 0; i < WORLD_SIZE; i++) {
        for (int j = 0; j < WORLD_SIZE; j++) {
            world[i][j] = '.'; // Empty space
        }
    }
}

// Function to render the world on the terminal
void render_world() {
    printf("\033[H\033[J"); // Clear the terminal screen
    for (int i = 0; i < WORLD_SIZE; i++) {
        for (int j = 0; j < WORLD_SIZE; j++) {
            printf("%c ", world[i][j]);
        }
        printf("\n");
    }
    fflush(stdout);
}

// Function to update the world array based on server data
void update_world_from_server(const char *data) {
    int x, y;
    char state;

    // Parse the server data (example format: "x y state")
    if (sscanf(data, "%d %d %c", &x, &y, &state) == 3) {
        if (x >= 0 && x < WORLD_SIZE && y >= 0 && y < WORLD_SIZE) {
            world[x][y] = state;
        }
    }
}

// Function to simulate receiving data from the server and rendering it
void run_renderer(int client_socket) {
    char buffer[BUFFER_SIZE];

    clear_world(); // Initialize the world

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (bytes_received <= 0) {
            printf("Disconnected from server.\n");
            break;
        }

        buffer[bytes_received] = '\0';
        update_world_from_server(buffer);
        render_world();
    }
}