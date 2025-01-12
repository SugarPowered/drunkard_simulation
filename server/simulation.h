//
// Created by Denis on 27. 12. 2024.
//

#ifndef SIMULATION_H
#define SIMULATION_H
#include <stdbool.h>

// Maximum size for simulation world dimensions
#define MAX_WORLD_SIZE 100

// Structure to represent the state of the simulation
typedef struct {
    bool in_menu; // True if in menu, false if in simulation mode
    bool interactive_mode; // True for interactive mode, false for summary mode

    int world_width; // Width of the world
    int world_height; // Height of the world

    int max_steps; // Maximum steps the walker can take
    int num_replications; // Number of replications

    double move_probabilities[4]; // Probabilities for up, down, left, right (must sum to 1)

    char results_file[256]; // File name to save results

    bool obstacles[MAX_WORLD_SIZE][MAX_WORLD_SIZE]; // Grid for obstacles (true = obstacle)
} simulation_state_t;

extern simulation_state_t global_simulation_state;

// Function prototypes
void initialize_simulation();
void reset_simulation();
void print_simulation_state();
void process_client_input_locally(const char *input);
simulation_state_t *get_simulation_state();
void perform_replications(FILE *file);


#endif //SIMULATION_H