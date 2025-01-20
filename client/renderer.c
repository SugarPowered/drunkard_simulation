#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "renderer.h"
#include "../sockets-lib/socket.h"

#define BUFFER_SIZE 1024

extern simulation_state_t global_simulation_state;

void clear_world() {
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            if (i == global_simulation_state.world_height / 2 &&
                j == global_simulation_state.world_width / 2) {
                global_simulation_state.world[i][j] = CENTER_WORLD;
            } else {
                global_simulation_state.world[i][j] = SPACE;
            }
        }
    }
}

void render_world() {
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            printf("%s", global_simulation_state.world[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void update_world_entire(int height, int width, char **lines) {
    for (int i = 0; i < height; i++) {
        int col = 0;
        char *saveptr = NULL;
        char *token = strtok_r(lines[i], " \n", &saveptr);
        while (token && col < width) {
            global_simulation_state.world[i][col] = strdup(token);
            token = strtok_r(NULL, " \n", &saveptr);
            col++;
        }
    }
}


void update_world_from_server(const char *data) {
    // Old code was for partial steps. We'll skip that approach now.
    // In the new approach, we won't do partial updates.
    // We'll do update_world_entire(...) instead in the client main loop.
}
