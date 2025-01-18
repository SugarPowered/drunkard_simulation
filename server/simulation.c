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

void initialize_simulation() {
    simulation_state_t state = global_simulation_state;

    for (int i = 0; i < MAX_WORLD_SIZE; ++i) {
        for (int j = 0; j < MAX_WORLD_SIZE; ++j) {
            state.obstacles[i][j] = false;
        }
    }

    FILE *result_file = fopen(state.results_file, "w");
    if (!result_file) {
        fprintf(stderr, "Neuspesne otvorenie vysledkoveho suboru.\n");
        return;
    }

    for (int i = 0; i < state.num_replications; ++i) {
      perform_replications(result_file);
    }

    fclose(result_file);
    printf("Vysledky ulozene do %s.\n", global_simulation_state.results_file);
    print_simulation_state();
    return;
}

void reset_simulation() {
    initialize_simulation();
}

void print_simulation_state() {
    printf("Stav Simulacie:\n");
    printf("  V Menu: %s\n", global_simulation_state.in_menu ? "Ano" : "Nie");
    printf("  Mod: %s\n", global_simulation_state.interactive_mode ? "Interaktivny" : "Sumarny");
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
    snprintf(global_simulation_state.results_file, sizeof(global_simulation_state.results_file), "%s", token);

    global_simulation_state.in_menu = false;
    global_simulation_state.interactive_mode = true;

    free(input_copy);
    initialize_simulation();
}

simulation_state_t *get_simulation_state() {
    return &global_simulation_state;
}

//void write_to_buffer(const char *data) {
//    static size_t current_position = 0;
//
//    size_t data_length = strlen(data);
//    if (current_position + data_length < BUFF_DATA_SIZE) {
//        memcpy(&file_buff[current_position], data, data_length);
//        current_position += data_length;
//    } else {
//        //fprintf(stderr, "Buffer overflow, data exceeds buffer size.\n");
//    }
//}

void perform_replications(FILE *file) {
  	static char buffer_data[BUFF_DATA_SIZE];

    fprintf(file, "Výsledky replikácie:\n");
//    write_to_buffer("Výsledky replikácie:\n");
    for (int x = 0; x < global_simulation_state.world_width; ++x) {
        for (int y = 0; y < global_simulation_state.world_height; ++y) {
            if (global_simulation_state.obstacles[x][y]) continue;

            fprintf(file, "Začiatok z (%d, %d):\n", x, y);
            snprintf(buffer_data, sizeof(buffer_data), "Začiatok z (%d, %d):\n", x, y);
//			write_to_buffer(buffer_data);

            for (int i = 0; i < global_simulation_state.num_replications; ++i) {
                int steps = 0;
                int cx = x, cy = y;

                while (steps < global_simulation_state.max_steps && !(cx == 0 && cy == 0)) {
                    double rand_val = (double)rand() / RAND_MAX;
                    if (rand_val < global_simulation_state.move_probabilities[0]) cy--;
                    else if (rand_val < global_simulation_state.move_probabilities[0] + global_simulation_state.move_probabilities[1]) cy++;
                    else if (rand_val < global_simulation_state.move_probabilities[0] + global_simulation_state.move_probabilities[1] + global_simulation_state.move_probabilities[2]) cx--;
                    else cx++;

                    steps++;
                }

                fprintf(file, "  Replikácia %d: krok %d \n", i + 1, steps);
                snprintf(buffer_data, sizeof(buffer_data), "  Replikácia %d: krok %d \n", i + 1, steps);
//				write_to_buffer(buffer_data);
            }
        }
    }
}