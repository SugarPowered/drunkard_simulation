#pragma once

#include "simulation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static simulation_state_t global_simulation_state;

// Initialize the simulation state with socket input values
void initialize_simulation(simulation_state_t *state) {

    state = &global_simulation_state;

    // Initialize obstacles to false
    for (int i = 0; i < MAX_WORLD_SIZE; ++i) {
        for (int j = 0; j < MAX_WORLD_SIZE; ++j) {
            state->obstacles[i][j] = false;
        }
    }

    for (int i = 0; i < state->num_replications; ++i) {
      perform_replications(state);
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

// Update simulation global state based on socket input
void process_client_input_locally(simulation_state_t *state, const char *input) {
	char *token;
    char *input_copy = calloc(strlen(input) + 1, sizeof(char)); // Duplicate input to avoid modifying original string
    strcpy(input_copy, input);
    const char delimiter[] = ";";

    // Skip the first token (which is "START_SIMULATION")
    token = strtok(input_copy, delimiter); // First token (should be "START_SIMULATION")
    printf("curr token: %s \n", token);

    // Second token: world width
    token = strtok(NULL, delimiter);
    printf("curr token: %s \n", token);
    global_simulation_state.world_width = atoi(token);

    // Third token: world height
    token = strtok(NULL, delimiter);
    printf("curr token: %s \n", token);
    global_simulation_state.world_height = atoi(token);

    // Fourth token: number of replications
    token = strtok(NULL, delimiter);
    printf("curr token: %s \n", token);
    global_simulation_state.num_replications = atoi(token);

    // Fifth token: probabilities (comma separated)
    token = strtok(NULL, delimiter);
    printf("curr token: %s \n", token);
    char *prob_token = strtok(token, ",");
    for (int i = 0; i < 4 && prob_token != NULL; i++) {
        global_simulation_state.move_probabilities[i] = atof(prob_token);
        prob_token = strtok(NULL, ",");
        printf("curr token: %s \n", prob_token);
    }

    // Sixth token: max steps
    token = strtok(NULL, delimiter);
    printf("curr token: %s \n", token);
    global_simulation_state.max_steps = atoi(token);

    // Seventh token: output file name
    token = strtok(NULL, delimiter);
    printf("curr token: %s \n", token);
    snprintf(global_simulation_state.results_file, sizeof(global_simulation_state.results_file), "%s", token);

    // Set in_menu and interactive_mode
    global_simulation_state.in_menu = false;
    global_simulation_state.interactive_mode = true;

    free(input_copy); // Free the duplicated string
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