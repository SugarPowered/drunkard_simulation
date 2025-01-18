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

// Prompts user (H)ost or (J)oin?
static int ask_user_for_mode() {
    char choice;
    printf("\nWould you like to host a new server (H) or join an existing server (J)?");
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


            char buffer[BUFFER_SIZE];
            while (1) {
                memset(buffer, 0, sizeof(buffer));

                int bytes_received = receive_from_server(socket_fd, buffer, sizeof(buffer));
                if (bytes_received <= 0) {
                    printf("Server disconnected or error.\n");
                    break;
                }

                if (strncmp(buffer, "SIMULATION_COMPLETED:", 21) == 0) {
                    printf("PRISLI DATA O DOKONCENI SIMULACIE, SPRACUVAM DO SUBORU\n");
                    break;
                }

                printf("Nespracovana sprava zo servera: %s\n", buffer);
                display_menu();
            }
            close_connection();
        }
    }
    else {
        // === JOIN existing server mode ===
        printf("Enter the server port to connect: ");
        scanf("%d", &chosen_port);

        int socket_fd = connect_to_server("127.0.0.1", chosen_port);
        if (socket_fd < 0) {
            fprintf(stderr, "ERROR: Could not connect to server on port %d.\n", chosen_port);
            return 1;
        }
        client_socket = socket_fd;


        // *** Existing loop ***
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = receive_from_server(socket_fd, buffer, sizeof(buffer));
            if (bytes_received <= 0) {
                printf("Server disconnected or error.\n");
                break;
            }

            if (strncmp(buffer, "SIMULATION_COMPLETED:", 21) == 0) {
                printf("PRISLI DATA O DOKONCENI SIMULACIE, SPRACUVAM DO SUBORU\n");
                break;
            }
            printf("Nespracovana sprava zo servera: %s\n", buffer);
            display_menu();
        }
        close_connection();
    }
    return 0;
}
