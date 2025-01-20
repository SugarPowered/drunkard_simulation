#ifndef SIMULATION_H
#define SIMULATION_H
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define BUFF_DATA_SIZE 1003
#define BUFFER_SIZE 1024

#define MOVE_UP " ^^ "
#define MOVE_DOWN " VV "
#define MOVE_LEFT " << "
#define MOVE_RIGHT " >> "
#define WALKER " CH "
#define OBSTACLE " || "
#define SPACE " .. "
#define CENTER_WORLD " SS "

typedef struct {
    bool in_menu;

    int world_width;
    int world_height;

    int max_steps;
    int has_obstacles;
    int obstacles_count;

    int is_interactive;
    int num_replications;

    double move_probabilities[4];

    char results_file[256];

    char*** world;

    double* average_probablity;
    double* steps_count;
} simulation_state_t;

extern simulation_state_t global_simulation_state;
static char buff[BUFF_DATA_SIZE];

// void place_obstacle(simulation_state_t *state);
void initialize_simulation(int client_socket);
void reset_simulation(int client_socket);
void print_simulation_state();
void process_client_input_locally(const char *input, int client_socket);
simulation_state_t *get_simulation_state();
void print_world();
void execute_simulation(FILE *file, int client_socket);

#endif //SIMULATION_H
