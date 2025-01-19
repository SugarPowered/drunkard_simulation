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

//void update_world(simulation_state_t *state) {
//  if (state->is_interactive) {
////    update_world_trajectory(state);
//  } else {
////    update_world_summary(state);
//  }
//}
//
//void place_obstacle(simulation_state_t *state) {
//  int center_x = state->world_width / 2;
//  int center_y = state->world_height / 2;
//
//  for (int i = 0; i < state->obstacles_count; i++) {
//    int x, y;
//
//    do {
//      x = rand() % state->world_width;
//      y = rand() % state->world_height;
//    } while ((x == center_x && y == center_y) || strcmp(state->world[x][y], OBSTACLE) == 0 || strcmp(state->world[x][y], WALKER) == 0);
//    state->world[x][y] = OBSTACLE;
//  }
//
//  update_world(state);
//}


void initialize_simulation() {
    if (global_simulation_state.in_menu) {
        printf("Sim State is in menu!\n");
        return;
    }

    simulation_state_t state = global_simulation_state;
    state.obstacles_count = (int)(state.world_width * state.world_height) * (0.2);

    state.world = malloc(state.world_height * sizeof(char**));
	for (int i = 0; i < state.world_height; i++) {
    	state.world[i] = malloc(state.world_width * sizeof(char*));
	}

    for (int i = 0; i < state.world_height; i++) {
        for (int j = 0; j < state.world_width; j++) {
            if (i == state.world_height/2 && j == state.world_width/2) {
              state.world[i][j] = CENTER_WORLD;
            }
            state.world[i][j] = SPACE;
        }
    }

    print_world();

    FILE *result_file = fopen(state.results_file, "w");
    if (!result_file) {
        fprintf(stderr, "Neuspesne otvorenie vysledkoveho suboru.\n");
        return;
    }

    for (int i = 0; i < state.num_replications; ++i) {
      perform_replication(result_file);
    }

    fclose(result_file);
    printf("Vysledky ulozene do %s.\n", global_simulation_state.results_file);
    print_simulation_state();

//    for (int i = 0; i < state.world_height; i++) {
//    	free(state.world[i]);
//	}
//	free(state.world);

    return;
}

void reset_simulation() {
    initialize_simulation();
}

void print_simulation_state() {
    printf("Stav Simulacie:\n");
    printf("  V Menu: %s\n", global_simulation_state.in_menu ? "Ano" : "Nie");
    printf("  Mod: %s\n", global_simulation_state.is_interactive ? "Interaktivny" : "Sumarny");
    printf("  Rozmery sveta: %dx%d\n", global_simulation_state.world_width, global_simulation_state.world_height);
    printf("  Max krokov: %d\n", global_simulation_state.max_steps);
    printf("  Pocet replikacii: %d\n", global_simulation_state.num_replications);
    printf("  Pravdepodobnosti pohybov: HORE=%.2f, DOLE=%.2f, VLAVO=%.2f, VPRAVO=%.2f\n",
           global_simulation_state.move_probabilities[0], global_simulation_state.move_probabilities[1],
           global_simulation_state.move_probabilities[2], global_simulation_state.move_probabilities[3]);
    printf("  Subor vysledkov: %s\n", global_simulation_state.results_file);
}

void process_client_input_locally(const char *input) {
	char *token;
    char *input_copy = calloc(strlen(input) + 1, sizeof(char));
    strcpy(input_copy, input);
    const char delimiter[] = ";";

    token = strtok(input_copy, delimiter);

    token = strtok(NULL, delimiter);
    global_simulation_state.world_width = atoi(token);

    token = strtok(NULL, delimiter);
    global_simulation_state.world_height = atoi(token);

    token = strtok(NULL, delimiter);
    global_simulation_state.num_replications = atoi(token);

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
    snprintf(global_simulation_state.results_file, sizeof(global_simulation_state.results_file), "%s", token);

    global_simulation_state.in_menu = false;

    free(input_copy);
    initialize_simulation();
}

simulation_state_t *get_simulation_state() {
    return &global_simulation_state;
}

void print_world() {
  	for (int i = 0; i < global_simulation_state.world_height; i++) {
    	for (int j = 0; j < global_simulation_state.world_width; j++) {
      		printf("%s", global_simulation_state.world[i][j]);
    	}
  	}
}

void perform_replication(FILE *file) {

}