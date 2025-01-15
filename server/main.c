#pragma once

#include "server.h"
#include "simulation.h"

int main() {
    simulation_state_t *state = get_simulation_state();

    initialize_server(PORT);

    initialize_simulation();
    return 0;
}


