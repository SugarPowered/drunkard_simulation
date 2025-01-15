#pragma once

#include "simulation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

simulation_state_t global_simulation_state = {
    .world_width = 0,
    .world_height = 0,
    .num_replications = 0,
    .move_probabilities = {0},
    .max_steps = 0,
    .results_file = "",
    .in_menu = false,
    .interactive_mode = false
};

// Initialize the simulation state with socket input values
void initialize_simulation() {
    simulation_state_t state = global_simulation_state;

    // Initialize obstacles to false
    for (int i = 0; i < MAX_WORLD_SIZE; ++i) {
        for (int j = 0; j < MAX_WORLD_SIZE; ++j) {
            state.obstacles[i][j] = false;
        }
    }

    FILE *result_file = fopen(state.results_file, "w");
    if (!result_file) {
        fprintf(stderr, "Failed to open results file.\n");
        return;
    }

    for (int i = 0; i < state.num_replications; ++i) {
      perform_replications(result_file);
    }

    fclose(result_file);
    printf("Results saved to %s.\n", global_simulation_state.results_file);
    print_simulation_state();
    return;
}

// Reset the simulation state to default values
void reset_simulation() {
    initialize_simulation();
}

// Print the current simulation state
void print_simulation_state() {
    printf("Simulation State:\n");
    printf("  In Menu: %s\n", global_simulation_state.in_menu ? "Yes" : "No");
    printf("  Mode: %s\n", global_simulation_state.interactive_mode ? "Interactive" : "Summary");
    printf("  World Dimensions: %dx%d\n", global_simulation_state.world_width, global_simulation_state.world_height);
    printf("  Max Steps: %d\n", global_simulation_state.max_steps);
    printf("  Replications: %d\n", global_simulation_state.num_replications);
    printf("  Move Probabilities: Up=%.2f, Down=%.2f, Left=%.2f, Right=%.2f\n",
           global_simulation_state.move_probabilities[0], global_simulation_state.move_probabilities[1],
           global_simulation_state.move_probabilities[2], global_simulation_state.move_probabilities[3]);
    printf("  Results File: %s\n", global_simulation_state.results_file);
}

// Update simulation global state based on socket input
void process_client_input_locally(const char *input) {
	char *token;
    char *input_copy = calloc(strlen(input) + 1, sizeof(char)); // Duplicate input to avoid modifying original string
    strcpy(input_copy, input);
    const char delimiter[] = ";";

    // Skip the first token (which is "START_SIMULATION")
    token = strtok(input_copy, delimiter); // First token (should be "START_SIMULATION")
    //printf("curr token START: %s \n", token);

    // Second token: world width
    token = strtok(NULL, delimiter);
    //printf("curr token WIDTH: %s \n", token);
    global_simulation_state.world_width = atoi(token);

    // Third token: world height
    token = strtok(NULL, delimiter);
    //printf("curr token HEIGHT: %s \n", token);
    global_simulation_state.world_height = atoi(token);

    // Fourth token: number of replications
    token = strtok(NULL, delimiter);
    //printf("curr token REPL: %s \n", token);
    global_simulation_state.num_replications = atoi(token);

    // Fifth token: probabilities (comma separated)
    //token = strtok(NULL, delimiter);
    //char *prob_token = strtok(token, ",");
    //printf("curr token PROPBS: %s \n", token);
    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, ",");
        global_simulation_state.move_probabilities[i] = atof(token);
        //("curr token I%d: %s \n", i, token);
    }
    token = strtok(NULL, delimiter);
    global_simulation_state.move_probabilities[3] = atof(token);
    //printf("curr token I3: %s \n", token);

    // Sixth token: max steps
    token = strtok(NULL, delimiter);
    //printf("curr token STEPS: %s \n", token);
    global_simulation_state.max_steps = atoi(token);

    // Seventh token: output file name
    token = strtok(NULL, delimiter);
    //printf("curr token OUTFILE: %s \n", token);
    snprintf(global_simulation_state.results_file, sizeof(global_simulation_state.results_file), "%s", token);

    // Set in_menu and interactive_mode
    global_simulation_state.in_menu = false;
    global_simulation_state.interactive_mode = true;

    free(input_copy); // Free the duplicated string
    initialize_simulation();
}

// Retrieve the global simulation state
simulation_state_t *get_simulation_state() {
    return &global_simulation_state;
}

void write_to_buffer(const char *data) {
    static size_t current_position = 0;

    size_t data_length = strlen(data);
    if (current_position + data_length < BUFF_DATA_SIZE) {
        memcpy(&file_buff[current_position], data, data_length);
        current_position += data_length;
    } else {
        //fprintf(stderr, "Buffer overflow, data exceeds buffer size.\n");
    }
}

void perform_replications(FILE *file) {
  	static char buffer_data[BUFF_DATA_SIZE];

    fprintf(file, "Replication results:\n");
    write_to_buffer("Replication results:\n");
    for (int x = 0; x < global_simulation_state.world_width; ++x) {
        for (int y = 0; y < global_simulation_state.world_height; ++y) {
            if (global_simulation_state.obstacles[x][y]) continue;

            fprintf(file, "Starting from (%d, %d):\n", x, y);
            snprintf(buffer_data, sizeof(buffer_data), "Starting from (%d, %d):\n", x, y);
			write_to_buffer(buffer_data);

            // Simulate K steps for each point
            for (int i = 0; i < global_simulation_state.num_replications; ++i) {
                int steps = 0;
                int cx = x, cy = y;

                while (steps < global_simulation_state.max_steps && !(cx == 0 && cy == 0)) {
                    double rand_val = (double)rand() / RAND_MAX;
                    if (rand_val < global_simulation_state.move_probabilities[0]) cy--; // Up
                    else if (rand_val < global_simulation_state.move_probabilities[0] + global_simulation_state.move_probabilities[1]) cy++; // Down
                    else if (rand_val < global_simulation_state.move_probabilities[0] + global_simulation_state.move_probabilities[1] + global_simulation_state.move_probabilities[2]) cx--; // Left
                    else cx++; // Right

                    steps++;
                }

                fprintf(file, "  Replication %d: %d steps\n", i + 1, steps);
                snprintf(buffer_data, sizeof(buffer_data), "  Replication %d: %d steps\n", i + 1, steps);
				write_to_buffer(buffer_data);
            }
        }
    }
}