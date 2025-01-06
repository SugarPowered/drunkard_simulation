//
// Created by Denis on 27. 12. 2024.
//

#ifndef SIMULATION_H
#define SIMULATION_H
#pragma once
#include <stdbool.h>

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
  int propabilities[4]; // Needs to be Total 1 / Cover in input processor
  int max_steps;
  char * file_to_save;
  bool interactive_world;
  bool obstacle_world;
  char * file_to_load;
} simulation;

// Inicialize struct data
simulation * simulation_init(int world_x, int world_y, int replications, int propabilities[4],
  int max_steps, char * file_to_save, bool interactive_world, bool obstacle_world, char * file_to_load);

// Based on whether  is interactive or sumarry mode will draw simulation
void draw_simulation(simulation * simulation);

// Make step for walker
void step_simulation(simulation * simulation);

// Load data from file
void load_from_file(simulation * simulation, char * file_to_load);

// Save simulation result to file
void save_to_file(simulation * simulation, char * file_to_save);

#endif //SIMULATION_H