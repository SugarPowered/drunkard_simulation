#pragma once

#include "simulation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static simulation_state_t global_simulation_state;

// Initialize the simulation state with default values
void initialize_simulation(simulation_state_t *state) {
    state->in_menu = true;
    state->interactive_mode = true;

    state->world_width = 10;
    state->world_height = 10;

    state->max_steps = 100;
    state->num_replications = 10;

    state->move_probabilities[0] = 0.25; // Up
    state->move_probabilities[1] = 0.25; // Down
    state->move_probabilities[2] = 0.25; // Left
    state->move_probabilities[3] = 0.25; // Right

    strncpy(state->results_file, "simulation_results.txt", sizeof(state->results_file));

    // Initialize obstacles to false
    for (int i = 0; i < MAX_WORLD_SIZE; ++i) {
        for (int j = 0; j < MAX_WORLD_SIZE; ++j) {
            state->obstacles[i][j] = false;
        }
    }
}

// Reset the simulation state to default values
void reset_simulation(simulation_state_t *state) {
    initialize_simulation(state);
}

// Print the current simulation state
void print_simulation_state(const simulation_state_t *state) {
    printf("Simulation State:\n");
    printf("  In Menu: %s\n", state->in_menu ? "Yes" : "No");
    printf("  Mode: %s\n", state->interactive_mode ? "Interactive" : "Summary");
    printf("  World Dimensions: %dx%d\n", state->world_width, state->world_height);
    printf("  Max Steps: %d\n", state->max_steps);
    printf("  Replications: %d\n", state->num_replications);
    printf("  Move Probabilities: Up=%.2f, Down=%.2f, Left=%.2f, Right=%.2f\n",
           state->move_probabilities[0], state->move_probabilities[1],
           state->move_probabilities[2], state->move_probabilities[3]);
    printf("  Results File: %s\n", state->results_file);
}

// Update simulation state based on socket input
void process_client_input_locally(simulation_state_t *state, const char *input) {
    if (strncmp(input, "SET_MENU", 8) == 0) {
        state->in_menu = true;
        printf("Switched to menu mode.\n");
    } else if (strncmp(input, "SET_SIM", 7) == 0) {
        state->in_menu = false;
        printf("Switched to simulation mode.\n");
    } else if (strncmp(input, "SET_MODE_INTERACTIVE", 20) == 0) {
        state->interactive_mode = true;
        printf("Switched to interactive mode.\n");
    } else if (strncmp(input, "SET_MODE_SUMMARY", 16) == 0) {
        state->interactive_mode = false;
        printf("Switched to summary mode.\n");
    } else if (strncmp(input, "SET_DIMENSIONS", 14) == 0) {
        int width, height;
        if (sscanf(input + 15, "%d %d", &width, &height) == 2) {
            state->world_width = width;
            state->world_height = height;
            printf("Updated world dimensions to %dx%d.\n", width, height);
        } else {
            printf("Invalid dimensions input.\n");
        }
    } else if (strncmp(input, "SET_MAX_STEPS", 13) == 0) {
        int max_steps;
        if (sscanf(input + 14, "%d", &max_steps) == 1) {
            state->max_steps = max_steps;
            printf("Updated max steps to %d.\n", max_steps);
        } else {
            printf("Invalid max steps input.\n");
        }
    } else if (strncmp(input, "SET_REPLICATIONS", 16) == 0) {
        int replications;
        if (sscanf(input + 17, "%d", &replications) == 1) {
            state->num_replications = replications;
            printf("Updated number of replications to %d.\n", replications);
        } else {
            printf("Invalid replications input.\n");
        }
    } else {
        printf("Unknown input command: %s\n", input);
    }
}

// Retrieve the global simulation state
simulation_state_t *get_simulation_state() {
    return &global_simulation_state;
}

void perform_replications(simulation_state_t *state) {
    FILE *result_file = fopen(state->results_file, "w");
    if (!result_file) {
        fprintf(stderr, "Failed to open results file.\n");
        return;
    }

    fprintf(result_file, "Replication results:\n");
    for (int x = 0; x < state->world_width; ++x) {
        for (int y = 0; y < state->world_height; ++y) {
            if (state->obstacles[x][y]) continue;

            fprintf(result_file, "Starting from (%d, %d):\n", x, y);

            // Simulate K steps for each point
            for (int i = 0; i < state->num_replications; ++i) {
                int steps = 0;
                int cx = x, cy = y;

                while (steps < state->max_steps && !(cx == 0 && cy == 0)) {
                    double rand_val = (double)rand() / RAND_MAX;
                    if (rand_val < state->move_probabilities[0]) cy--; // Up
                    else if (rand_val < state->move_probabilities[0] + state->move_probabilities[1]) cy++; // Down
                    else if (rand_val < state->move_probabilities[0] + state->move_probabilities[1] + state->move_probabilities[2]) cx--; // Left
                    else cx++; // Right

                    steps++;
                }

                fprintf(result_file, "  Replication %d: %d steps\n", i + 1, steps);
            }
        }
    }

    fclose(result_file);
    printf("Results saved to %s.\n", state->results_file);
}