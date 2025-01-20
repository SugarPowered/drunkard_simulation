#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../server/server.h"
#include "../sockets-lib/socket.h"
#include "../pipe-lib/pipe.h"
#include "client.h"
#include "menu.h"

#define BUFFER_SIZE 2048

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../server/server.h"
#include "../sockets-lib/socket.h"
#include "../pipe-lib/pipe.h"
#include "client.h"
#include "menu.h"

// ...
#define BUFFER_SIZE 2048

void handle_server_message(const char *msg, int socket_fd) {
    char copy[BUFFER_SIZE];
    strncpy(copy, msg, sizeof(copy));
    copy[sizeof(copy) - 1] = '\0';

    char *token = strtok(copy, "|");
    if (!token) return;

    if (strcmp(token, "INFO") == 0) {
        token = strtok(NULL, "|");
        if (!token) return;

        if (strcmp(token, "MENU") == 0) {
            display_menu();
        }
        else if (strcmp(token, "WORLD") == 0) {
            // parse height & width
            char *height_str = strtok(NULL, "|");
            char *width_str  = strtok(NULL, "\n");
            if (!height_str || !width_str) return;

            int h = atoi(height_str);
            int w = atoi(width_str);

            // 1) store them in global state
            //    If world already allocated, free it first
            if (global_simulation_state.world) {
                // free previous data
                for (int row = 0; row < global_simulation_state.world_height; row++) {
                    free(global_simulation_state.world[row]);
                }
                free(global_simulation_state.world);
                global_simulation_state.world = NULL;
            }

            global_simulation_state.world_height = h;
            global_simulation_state.world_width  = w;

            // 2) allocate
            global_simulation_state.world = malloc(h * sizeof(char**));
            for (int i = 0; i < h; i++) {
                global_simulation_state.world[i] = malloc(w * sizeof(char*));
                // Do not allocate the third level if you only store char pointers from strdup
            }

            // 3) read h lines from the socket
            char **lines = malloc(sizeof(char*) * h);
            for (int i = 0; i < h; i++) {
                lines[i] = calloc(1, BUFFER_SIZE);
                int bytes_read = read(socket_fd, lines[i], BUFFER_SIZE - 1);
                if (bytes_read <= 0) {
                    printf("Server disconnected or error reading row.\n");
                    // handle error or break
                    break;
                }
                lines[i][bytes_read] = '\0';
            }

            // 4) parse them into the local array
            update_world_entire(h, w, lines);

            // 5) now we can render
            render_world();

            // 6) free lines
            for (int i = 0; i < h; i++) {
                free(lines[i]);
            }
            free(lines);
        }
        else {
            // e.g. "INFO|something_else"
            printf("[SERVER->CLIENT] %s\n", msg);
        }
    }
    else {
        // Possibly "SIM_UPDATE" or other states
    }
}


void sim_loop(int socket_fd) {
    char buffer[BUFFER_SIZE];

    clear_world(); // Initialize local world
    render_world();

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = receive_from_server(socket_fd, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            printf("Server disconnected or error.\n");
            break;
        }

        handle_server_message(buffer, socket_fd);
    }
}

// Prompts user (H)ost or (J)oin?
static int ask_user_for_mode() {
    char choice;
    printf("\nWould you like to host a new server (H) or join an existing server (J)?: ");
    scanf(" %c", &choice);
    if (choice == 'H' || choice == 'h') return 1;
    return 0;
}

int main() {
    int is_host = ask_user_for_mode();
    int chosen_port = 0;

    if (is_host) {

        srand(time(NULL));
        int rnd = rand() % 100000;
        char pipe[24];
        snprintf(pipe, sizeof(pipe),
                 "DRUNKARD_%d_SIM", rnd);

        printf("Creating a new pipe %s\n", pipe);

        pipe_init(pipe);

        pid_t cpid = fork();
        if (cpid < 0) {
            perror("Failed to fork server\n");
            pipe_destroy(pipe);
            return 1;
        }
        else if (cpid == 0) {
            int fd_write = pipe_open_write(pipe);

            run_server_internally_with_fifo(0, fd_write);

            pipe_close(fd_write);
            _exit(0);
        }
        else {
            printf("Starting client in 1 second...\n");
            sleep(1);
            int fd_read = pipe_open_read(pipe);

            if (read(fd_read, &chosen_port, sizeof(chosen_port)) == -1) {
                fprintf(stderr, "ERROR: Did not receive ephemeral port from child.\n");
                exit(EXIT_FAILURE);
            }
            printf("Parent got ephemeral port: %d\n", chosen_port);

            pipe_close(fd_read);
            pipe_destroy(pipe);

            sleep(1);

            int socket_fd = connect_to_server("127.0.0.1", chosen_port);
            if (socket_fd < 0) {
                fprintf(stderr, "ERROR: Could not connect to server on port %d.\n", chosen_port);
                return 1;
            }
            client_socket = socket_fd;

            sim_loop(socket_fd);

            close_connection();
        }
    }
    else {
        printf("Enter the server port to connect: ");
        scanf("%d", &chosen_port);

        int socket_fd = connect_to_server("127.0.0.1", chosen_port);
        if (socket_fd < 0) {
            fprintf(stderr, "ERROR: Could not connect to server on port %d.\n", chosen_port);
            return 1;
        }
        client_socket = socket_fd;

        sim_loop(socket_fd);

        close_connection();
    }
    return 0;
}
