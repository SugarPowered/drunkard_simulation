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

void place_obstacle(simulation_state_t *state) {
  int center_x = state->world_width / 2;
  int center_y = state->world_height / 2;

  for (int i = 0; i < state->obstacles_count; i++) {
    int x, y;

    do {
      x = rand() % state->world_width;
      y = rand() % state->world_height;
    } while ((x == center_x && y == center_y) || strcmp(state->world[x][y], OBSTACLE) == 0 || strcmp(state->world[x][y], WALKER) == 0);
    state->world[x][y] = OBSTACLE;
  }

  //update_world(state);
}

void initialize_simulation() {
    if (global_simulation_state.in_menu) {
        printf("Sim State is in menu!\n");
        return;
    }

    printf("[SERVER] Initializing simulation...\n");

    global_simulation_state.obstacles_count = (int)(
        global_simulation_state.world_width * global_simulation_state.world_height * 0.2
    );

    global_simulation_state.world = malloc(global_simulation_state.world_height * sizeof(char**));
    if (!global_simulation_state.world) {
        perror("malloc world");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < global_simulation_state.world_height; i++) {
        global_simulation_state.world[i] = malloc(global_simulation_state.world_width * sizeof(char*));
        if (!global_simulation_state.world[i]) {
            perror("malloc world[i]");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
            if (i == global_simulation_state.world_height / 2 && j == global_simulation_state.world_width / 2) {
                global_simulation_state.world[i][j] = CENTER_WORLD;
            } else {
                global_simulation_state.world[i][j] = SPACE;
            }

        }
    }

    if(global_simulation_state.has_obstacles) {
      place_obstacle(&global_simulation_state);
    }

    FILE *result_file = fopen(global_simulation_state.results_file, "w");
    if (!result_file) {
        fprintf(stderr, "Neuspesne otvorenie vysledkoveho suboru.\n");
        return;
    }

    execute_simulation(result_file);

    fclose(result_file);

    printf("Vysledky ulozene do %s.\n", global_simulation_state.results_file);
//    print_simulation_state();
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

void reset_world() {
  for (int i = 0; i < global_simulation_state.world_height; i++) {
        for (int j = 0; j < global_simulation_state.world_width; j++) {
          	if (strcmp(global_simulation_state.world[i][j],OBSTACLE) == 0  ) {
                  break;
          	}
            if (i == global_simulation_state.world_height / 2 && j == global_simulation_state.world_width / 2) {
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
  double random_number = ((double) rand() / (double) RAND_MAX);
  double cumulative = 0.0;
  for (int i = 0; i < size; i++) {
    cumulative += probabilities[i];
    if (random_number < cumulative) {
      return i;
    }
  }
  return 0;
}

void write_to_buffer(const char *data) {
    if (strlen(buff) + strlen(data) + 1 < BUFF_DATA_SIZE) {
        strcat(buff, data);
        strcat(buff, "\n"); // Adding a newline for each entry
    } else {
        fprintf(stderr, "Buffer overflow detected!\n");
    }
}

void execute_simulation(FILE *file) {
  int new_position = 0;

  int center_x = global_simulation_state.world_width / 2;
  int center_y = global_simulation_state.world_height / 2;

  for (int i = 0; i < global_simulation_state.world_height; i++) {
    for (int j = 0; j < global_simulation_state.world_width; j++) {
      printf("Vypocet pre policko: [%d,%d] \n", i,j);
      for (int k = 0; k < global_simulation_state.num_replications; k++) {
        printf("Replikacia: %d \n", k);
        int new_x = i;
      	int new_y = j;

        if(strcmp(global_simulation_state.world[i][j], OBSTACLE) == 0) {
          continue;
        }
        global_simulation_state.world[i][j] = WALKER;

        for (int step = 0; step < global_simulation_state.max_steps; step++) {
      		int direction = choose_direction(global_simulation_state.move_probabilities, 4);

      		switch (direction) {
        		case 0:
					new_position = (new_y + 1) % global_simulation_state.world_height;
                    if(strcmp(global_simulation_state.world[new_x][new_position],OBSTACLE) != 0) {
                        global_simulation_state.world[new_x][new_y] = MOVE_UP;
                        new_y = new_position;
                    }
          			break; //up
        		case 1:
                    new_position = (new_y - 1 + global_simulation_state.world_height) % global_simulation_state.world_height;
					if(strcmp(global_simulation_state.world[new_x][new_position],OBSTACLE) != 0) {
                        global_simulation_state.world[new_x][new_y] = MOVE_DOWN;
                        new_y = new_position;
					}
          			break; //down
        		case 2:
                    new_position = (new_x + 1) % global_simulation_state.world_width;
                    if(strcmp(global_simulation_state.world[new_position][new_y],OBSTACLE) != 0) {
                        global_simulation_state.world[new_x][new_y] = MOVE_RIGHT;
                        new_x = new_position;
                    }
          			break; // right
        		case 3:
                    new_position = (new_x - 1 + global_simulation_state.world_width) % global_simulation_state.world_width;
                    if(strcmp(global_simulation_state.world[new_position][new_y],OBSTACLE) != 0) {
                        global_simulation_state.world[new_x][new_y] = MOVE_LEFT;
                        new_x = new_position;
                    }
          			break; //left
            }

            if(center_x == new_x && center_y == new_y) {
              break;
            }

            char update[256];
            snprintf(update, sizeof(update), "%d %d %c", new_x, new_y, *global_simulation_state.world[new_x][new_y]);
            write_to_buffer(update);
        }
        global_simulation_state.world[new_x][new_y] = WALKER;
        print_world();
        reset_world();
      }
    }
  }

  if (file) {
     fprintf(file, "%s", buff);
  }
}