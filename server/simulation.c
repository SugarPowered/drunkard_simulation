//
// Created by michal.subert on 10. 1. 2025.
//

#include "simulation.h"

void process_direction_probability(simulation *sim) {
  // todo process direction data from socket
}

void process_simulation_params(simulation *sim) {
  // todo process data from socket
}



void start_interactive_mode(simulation *sim) {
  printf("\nSpustam interaktivny mod...\n");
  //todo send_simulation_data(sim);
  exit(0);
}

void start_summary_mode(simulation *sim) {
  printf("\nSpustam sumarny mod...\n");
  for (int i = 0; i < sim->replications; i++) {
    //todo step_simulation(sim);
  }
  //todo save_to_file(sim, sim->file_to_save);
  exit(0);
}

simulation * create_simulation(int world_x, int world_y, int replications, int propabilities[4],
  int max_steps, char * file_to_save, bool interactive_world, bool obstacle_world, char * file_to_load) {
  simulation *sim = malloc(sizeof(simulation));

    // todo initialize grid or other structures as needed

    return sim;
}

void run_simulation(simulation *sim) {
    // todo simulation logic goes here (movement, collision detection)
}