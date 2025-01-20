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

static char g_accumulator[8192];   // a buffer to accumulate multiline data
static int  g_accum_len = 0;       // how many bytes we currently have
static int  g_in_world_block = 0;  // are we currently inside WORLD_HEADER..END_WORLD lines?

/**
 * Check in the chunk if we have a "WORLD_HEADER" or "END_WORLD".
 * We'll do a quick approach: we treat everything between them as lines to print.
 */
static void process_chunk_for_world(const char *chunk) {
    // Append 'chunk' to our static g_accumulator
    int chunk_len = strlen(chunk);
    if (g_accum_len + chunk_len >= (int)sizeof(g_accumulator)) {
        // safety check to avoid overflow
        fprintf(stderr, "Accumulator buffer overflow. Truncating.\n");
        chunk_len = (int)sizeof(g_accumulator) - g_accum_len - 1;
    }
    memcpy(g_accumulator + g_accum_len, chunk, chunk_len);
    g_accum_len += chunk_len;
    g_accumulator[g_accum_len] = '\0';

    // We can now search in g_accumulator for "WORLD_HEADER" or "END_WORLD"
    // and handle them.
    // For simplicity, let's do line-by-line tokenization:
    char *start = g_accumulator;
    char *line = NULL;

    while ((line = strchr(start, '\n'))) {
        // 'line' points to the newline char
        // we can temporarily replace it with '\0' to get a full line string
        *line = '\0';

        // now 'start' is the line text
        if (strstr(start, "WORLD_HEADER") != NULL) {
            // We entered a world block
            g_in_world_block = 1;
            printf("\n--- BEGIN WORLD BLOCK ---\n");
        }
        else if (strstr(start, "END_WORLD") != NULL) {
            // We finished a world block
            g_in_world_block = 0;
            printf("--- END WORLD BLOCK ---\n\n");
        }
        else if (g_in_world_block) {
            // This is a line of the world. Print it or parse further.
            printf("%s\n", start);
        }
        else {
            // This is some line outside the block.
            // You might just ignore it or print it as debug:
            // printf("[SERVER-other] %s\n", start);
        }

        // move 'start' beyond this line
        start = line + 1;
    }

    // Now, 'start' might be pointing to the middle of g_accumulator if
    // the last chunk ended without a newline. We shift the remainder to the front.

    int remainder = g_accum_len - (int)(start - g_accumulator);
    if (remainder > 0) {
        memmove(g_accumulator, start, remainder);
    }
    g_accum_len = remainder;
    g_accumulator[g_accum_len] = '\0';
}

/**
 * Our old handle_server_message still checks if something is "INFO|MENU" or "SIM_UPDATE|..."
 * But if we want to detect "WORLD_HEADER" etc. as raw text, let's intercept that first.
 */
void handle_server_message(const char *msg) {
    // 1) Check if the chunk might contain "WORLD_HEADER" or "END_WORLD"
    // If so, let's let process_chunk_for_world handle it and skip the old logic
    if (strstr(msg, "WORLD_HEADER") != NULL || strstr(msg, "END_WORLD") != NULL || g_in_world_block) {
        // We'll feed it into the multiline logic
        process_chunk_for_world(msg);
        return;
    }

    // Otherwise, do your old token-based logic
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
        } else {
            printf("[SERVER->CLIENT] %s\n", msg);
        }
    } else if (strcmp(token, "SIM_UPDATE") == 0) {
        // Second token: "<simulation_data>"
        token = strtok(NULL, "|");
        if (!token) return;

        printf("[CLIENT->SERVER] Received simulation update...\n");
        // Possibly update world from 'token' data
        // update_world_from_server(token);
        // render_world();
    } else {
        // fallback
        // printf("[SERVER->CLIENT] %s\n", msg);
    }
}

void sim_loop(int socket_fd) {
    char buffer[BUFFER_SIZE];

    clear_world();
    render_world();

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = receive_from_server(socket_fd, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            printf("Server disconnected or error.\n");
            break;
        }

        // Pass the chunk to handle_server_message
        handle_server_message(buffer);
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
