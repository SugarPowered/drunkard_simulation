#pragma once

#include "server.h"
#include "simulation.h"

int main() {
    simulation_state_t *state = get_simulation_state();

    // Initialize simulation state
    initialize_simulation(state);

    printf("Simulation initialized. Starting server...\n");

    // Start the server
    run_server();

    return 0;
}


