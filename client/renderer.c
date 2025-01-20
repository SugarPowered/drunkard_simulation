#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "renderer.h"
#include "../sockets-lib/socket.h"

#define WORLD_SIZE 20
#define BUFFER_SIZE 1024

extern simulation_state_t global_simulation_state;

void clear_world() {
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            if (i == global_simulation_state.world_height / 2 && j == global_simulation_state.world_width / 2) {
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

void update_world_from_server(const char *data) {
    int x, y;
    char state[10];

    if (sscanf(data, "%d %d %s", &x, &y, state) == 3) {
        printf("[CLIENT-X] %d [CLIENT-Y] %d [CLIENT-STATE] %s\n", x, y, state);
        if (x >= 0 && x < global_simulation_state.world_width && y >= 0 && y < global_simulation_state.world_height) {
            strncpy(global_simulation_state.world[x][y], state, sizeof(global_simulation_state.world[x][y]) - 1);
        }
    }
}