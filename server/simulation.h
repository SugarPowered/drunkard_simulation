#ifndef SIMULATION_H
#define SIMULATION_H
#include <stdbool.h>
#include <stdio.h>

#define MAX_WORLD_SIZE 100
#define BUFF_DATA_SIZE 1003

typedef struct {
    bool in_menu;
    bool interactive_mode;

    int world_width;
    int world_height;

    int max_steps;
    int num_replications;

    double move_probabilities[4];

    char results_file[256];

    bool obstacles[MAX_WORLD_SIZE][MAX_WORLD_SIZE];
} simulation_state_t;

extern simulation_state_t global_simulation_state;
static char file_buff[2048] = {0};

void initialize_simulation();
void reset_simulation();
void print_simulation_state();
void process_client_input_locally(const char *input);
simulation_state_t *get_simulation_state();
void perform_replications(FILE *file);


#endif //SIMULATION_H