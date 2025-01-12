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

// Function prototypes
void initialize_simulation(simulation_state_t *state);
void reset_simulation(simulation_state_t *state);
void print_simulation_state(const simulation_state_t *state);
void update_simulation_from_input(simulation_state_t *state, const char *input);
simulation_state_t *get_simulation_state();
void perform_replications(simulation_state_t *state);


#endif //SIMULATION_H
/*
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct world_data_struct {
    int ** world_average_steps;
    int ** world_data;
} world_data_struct;

typedef struct world_struct {
    int world_x;
    int world_y;
    world_data_struct world_data;
} world_struct;

typedef struct simulation {
    world_struct world;
    int replications;
    double probabilities[4]; // Needs to be Total 1 / Cover in input processor
    int max_steps;
    char * file_to_save;
    bool interactive_world;
    bool obstacle_world;
    char * file_to_load;
} simulation;

void input_direction_probability(simulation *sim);
void input_simulation_params(simulation *sim);
void print_ascii_menu_screen();

void start_interactive_mode(simulation *sim);
void start_summary_mode(simulation *sim);

// Inicialize struct data
simulation * create_simulation(int world_x, int world_y, int replications, int propabilities[4],
  int max_steps, char * file_to_save, bool interactive_world, bool obstacle_world, char * file_to_load);

// Run created simulation
void run_simulation(simulation * simulation);

// Based on whether  is interactive or sumarry mode will draw simulation
// SUGI: => draw should be done by client => renderer.c
void draw_simulation(simulation * simulation);

// Make step for walker
void step_simulation(simulation * simulation);

// Load data from file
void load_from_file(simulation * simulation, char * file_to_load);

// Save simulation result to file
void save_to_file(simulation * simulation, char * file_to_save);
*/