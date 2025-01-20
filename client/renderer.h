//
// Created by michal.subert on 10. 1. 2025.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "../server/simulation.h"

void render_simulation(const char *simulation_state);
void clear_world();
void render_world();
void update_world_from_server(const char *data);
void run_renderer(int client_socket);

#endif // RENDERER_H
