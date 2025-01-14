#pragma once

#include "server.h"
#include "simulation.h"

int main() {
    simulation_state_t *state = get_simulation_state();

    // Start the server
    initialize_server(PORT);

    //printf("Server started. Simulation initializing. ...\n");

    // Initialize simulation state
    //  initialize_simulation();
    return 0;
}


