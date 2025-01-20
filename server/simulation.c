#pragma once

#include "simulation.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STEPS 30

simulation_state_t global_simulation_state = {
        .world_width = 0,
        .world_height = 0,
        .num_replications = 0,
        .move_probabilities = {0},
        .max_steps = 0,
        .has_obstacles = 0,
        .obstacles_count = 0,
        .is_interactive = 0,
        .results_file = "",
        .in_menu = true,
};

void place_obstacle(simulation_state_t *state) {
    int center_x = state->world_width / 2;
    int center_y = state->world_height / 2;

    for (int i = 0; i < state->obstacles_count; i++) {
        int x, y;
        do {
            x = rand() % state->world_width;
            y = rand() % state->world_height;
        } while (
                (x == center_x && y == center_y)
                || strcmp(state->world[x][y], OBSTACLE) == 0
                || strcmp(state->world[x][y], WALKER) == 0
                );
        state->world[x][y] = OBSTACLE;
    }
}

void initialize_simulation(int client_socket) {
    if (global_simulation_state.in_menu) {
        printf("Sim State is in menu!\n");
        return;
    }

    printf("[SERVER] Initializing simulation...\n");

    // Setup obstacle count
    global_simulation_state.obstacles_count =
            (int)(global_simulation_state.world_width * global_simulation_state.world_height * 0.2);

    // Allocate the world
    global_simulation_state.world = malloc(global_simulation_state.world_height * sizeof(char**));
    if (!global_simulation_state.world) {
        perror("malloc world");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        global_simulation_state.world[i] =
                malloc(global_simulation_state.world_width * sizeof(char*));
        if (!global_simulation_state.world[i]) {
            perror("malloc world[i]");
            exit(EXIT_FAILURE);
        }
    }

    // Fill each cell
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            if (i == global_simulation_state.world_height/2 && j == global_simulation_state.world_width/2) {
                global_simulation_state.world[i][j] = CENTER_WORLD;
            } else {
                global_simulation_state.world[i][j] = SPACE;
            }
        }
    }

    // Optionally place obstacles
    if (global_simulation_state.has_obstacles) {
        place_obstacle(&global_simulation_state);
    }

    // Open result file
    FILE *result_file = fopen(global_simulation_state.results_file, "w");
    if (!result_file) {
        fprintf(stderr, "Neuspesne otvorenie vysledkoveho suboru.\n");
        return;
    }

    // Run the simulation
    execute_simulation(result_file, client_socket);

    fclose(result_file);
    printf("Vysledky ulozene do %s.\n", global_simulation_state.results_file);
    return;
}

void reset_simulation(int client_socket) {
    initialize_simulation(client_socket);
}

void print_simulation_state() {
    printf("Stav Simulacie:\n");
    printf("  V Menu: %s\n",   global_simulation_state.in_menu ? "Ano" : "Nie");
    printf("  Mod: %s\n",      global_simulation_state.is_interactive ? "Interaktivny" : "Sumarny");
    printf("  Rozmery sveta: %dx%d\n",
           global_simulation_state.world_width, global_simulation_state.world_height);
    printf("  Max krokov: %d\n", global_simulation_state.max_steps);
    printf("  Pocet replikacii: %d\n", global_simulation_state.num_replications);
    printf("  Pravdepodobnosti pohybov: HORE=%.2f, DOLE=%.2f, VLAVO=%.2f, VPRAVO=%.2f\n",
           global_simulation_state.move_probabilities[0],
           global_simulation_state.move_probabilities[1],
           global_simulation_state.move_probabilities[2],
           global_simulation_state.move_probabilities[3]);
    printf("  Subor vysledkov: %s\n", global_simulation_state.results_file);
}

void process_client_input_locally(const char *input, int client_socket) {
    char *input_copy = calloc(strlen(input)+1, sizeof(char));
    strcpy(input_copy, input);
    const char delimiter[] = ";";

    char *token = strtok(input_copy, delimiter); // START_SIMULATION perhaps
    token = strtok(NULL, delimiter);
    global_simulation_state.world_width = atoi(token);

    token = strtok(NULL, delimiter);
    global_simulation_state.world_height = atoi(token);

    token = strtok(NULL, delimiter);
    global_simulation_state.num_replications = atoi(token);

    // parse move probabilities
    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, ",");
        global_simulation_state.move_probabilities[i] = atof(token);
    }
    token = strtok(NULL, delimiter);
    global_simulation_state.move_probabilities[3] = atof(token);

    token = strtok(NULL, delimiter);
    global_simulation_state.max_steps = atoi(token);

    token = strtok(NULL, delimiter);
    global_simulation_state.has_obstacles = atoi(token);

    token = strtok(NULL, delimiter);
    global_simulation_state.is_interactive = atoi(token);

    token = strtok(NULL, delimiter);
    snprintf(global_simulation_state.results_file,
             sizeof(global_simulation_state.results_file), "%s", token);

    global_simulation_state.in_menu = false;

    free(input_copy);
    initialize_simulation(client_socket);
}

simulation_state_t *get_simulation_state() {
    return &global_simulation_state;
}

// Reset the world to just SPACE/CENTER, ignoring obstacles
void reset_world() {
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            if (strcmp(global_simulation_state.world[i][j], OBSTACLE) == 0) {
                // Keep the obstacle or break if you prefer skipping the entire row
                // but let's keep obstacles
                continue;
            }
            if (i == global_simulation_state.world_height/2 && j == global_simulation_state.world_width/2) {
                global_simulation_state.world[i][j] = CENTER_WORLD;
            } else {
                global_simulation_state.world[i][j] = SPACE;
            }
        }
    }
}

void print_world() {
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            printf("%s", global_simulation_state.world[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int choose_direction(const double probabilities[], int size) {
    double r = (double) rand() / RAND_MAX;
    double cumulative = 0.0;
    for (int i = 0; i < size; i++) {
        cumulative += probabilities[i];
        if (r < cumulative) {
            return i;
        }
    }
    return size-1; // fallback
}

/**
 * Send the entire 2D world to the client, row by row.
 */
void send_entire_world(int client_socket) {
    // We'll do a multi-part message:
    //   1) "INFO|WORLD|<height>|<width>\n"
    //   2) Then <height> lines, each containing the cells separated by spaces.
    char buffer[4096];

    // 1) Header line
    snprintf(buffer, sizeof(buffer),
             "INFO|WORLD|%d|%d\n",
             global_simulation_state.world_height,
             global_simulation_state.world_width);
    write(client_socket, buffer, strlen(buffer));

    // 2) Each row
    for (int i = 0; i < global_simulation_state.world_height; i++) {
        memset(buffer, 0, sizeof(buffer));
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            strncat(buffer, global_simulation_state.world[i][j],
                    sizeof(buffer) - strlen(buffer) - 1);
            strncat(buffer, " ", sizeof(buffer) - strlen(buffer) - 1);
        }
        strncat(buffer, "\n", sizeof(buffer) - strlen(buffer) - 1);
        write(client_socket, buffer, strlen(buffer));
    }

    // optional end marker
    // write(client_socket, "INFO|WORLD_END\n", 16);
}

void execute_simulation(FILE *file, int client_socket) {
    // We'll do a random walk for each cell, then send the entire 2D array to the client once
    // so it can be rendered. Modify as needed.

    int center_x = global_simulation_state.world_width / 2;
    int center_y = global_simulation_state.world_height / 2;

    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            // if there's an obstacle, skip or handle differently
            if (strcmp(global_simulation_state.world[i][j], OBSTACLE) == 0) {
                continue;
            }

            printf("[SERVER] Starting random walks at cell [%d, %d]\n", i, j);

            for (int rep = 0; rep < global_simulation_state.num_replications; rep++) {
                // place the walker there
                global_simulation_state.world[i][j] = WALKER;

                int wx = i, wy = j;

                // do up to max_steps
                for (int step = 0; step < global_simulation_state.max_steps; step++) {
                    int dir = choose_direction(global_simulation_state.move_probabilities, 4);

                    // Attempt the move
                    switch (dir) {
                        case 0: { // up
                            int new_y = (wy + 1) % global_simulation_state.world_height;
                            if (strcmp(global_simulation_state.world[wx][new_y], OBSTACLE) != 0) {
                                global_simulation_state.world[wx][wy] = MOVE_UP;
                                wy = new_y;
                            }
                            break;
                        }
                        case 1: { // down
                            int new_y = (wy - 1 + global_simulation_state.world_height)
                                        % global_simulation_state.world_height;
                            if (strcmp(global_simulation_state.world[wx][new_y], OBSTACLE) != 0) {
                                global_simulation_state.world[wx][wy] = MOVE_DOWN;
                                wy = new_y;
                            }
                            break;
                        }
                        case 2: { // right
                            int new_x = (wx + 1) % global_simulation_state.world_width;
                            if (strcmp(global_simulation_state.world[new_x][wy], OBSTACLE) != 0) {
                                global_simulation_state.world[wx][wy] = MOVE_RIGHT;
                                wx = new_x;
                            }
                            break;
                        }
                        case 3: { // left
                            int new_x = (wx - 1 + global_simulation_state.world_width)
                                        % global_simulation_state.world_width;
                            if (strcmp(global_simulation_state.world[new_x][wy], OBSTACLE) != 0) {
                                global_simulation_state.world[wx][wy] = MOVE_LEFT;
                                wx = new_x;
                            }
                            break;
                        }
                    }

                    // If reached the center, break early
                    if (wx == center_x && wy == center_y) {
                        break;
                    }
                }

                // Mark final position as walker
                global_simulation_state.world[wx][wy] = WALKER;

                // Send the entire world to the client so it can be rendered
                send_entire_world(client_socket);

                // Also write results to file if you like
                fprintf(file, "Replication %d from start[%d, %d] ended at [%d, %d]\n",
                        rep+1, i, j, wx, wy);

                // reset the array to SPACE, except obstacles remain
                reset_world();
            }
        }
    }
}
