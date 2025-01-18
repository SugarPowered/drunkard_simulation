#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>      // for srand/time
#include <sys/types.h> // for pid_t
#include "../server/server.h"
#include "../sockets-lib/socket.h"
#include "../pipe-lib/pipe.h"
#include "client.h"
#include "menu.h"

#define BUFFER_SIZE 2048

// Prompts user (H)ost or (J)oin?
static int ask_user_for_mode() {
    char choice;
    printf("\nWould you like to host a new server (H) or join an existing server (J)? ");
    scanf(" %c", &choice);
    if (choice == 'H' || choice == 'h') return 1;
    return 0;
}

int main() {
    int is_host = ask_user_for_mode();
    int chosen_port = 0;

    if (is_host) {
        // 1) Generate a unique FIFO path
        // e.g. /tmp/DRUNKARD_5739_1234
        srand(time(NULL));
        int rnd = rand() % 100000;  // or some bigger range
        pid_t pid_self = getpid();

        // We'll store the generated path in fifo_path
        char fifo_path[256];
        snprintf(fifo_path, sizeof(fifo_path),
                 "/tmp/DRUNKARD_%d_%d_SIM", (int)pid_self, rnd);

        // 2) Create the named pipe (mkfifo)
        pipe_init(fifo_path);

        // 3) Fork the child to run the server
        pid_t cpid = fork();
        if (cpid < 0) {
            perror("Failed to fork server");
            // If fork fails, clean up the FIFO
            pipe_destroy(fifo_path);
            return 1;
        }
        else if (cpid == 0) {
            // === CHILD PROCESS ===
            // We'll open the FIFO in *write* mode
            int fd_write = pipe_open_write(fifo_path);

            // Call a special server function that:
            // - picks a random ephemeral port
            // - writes that port to fd_write
            run_server_internally_with_fifo(0, fd_write);

            // If we ever return from run_server_internally_with_fifo, close & exit
            pipe_close(fd_write);
            _exit(0);
        }
        else {
            // === PARENT PROCESS (CLIENT) ===
            // We'll open the FIFO in *read* mode
            int fd_read = pipe_open_read(fifo_path);

            // Next, we read the chosen ephemeral port from the FIFO
            if (read(fd_read, &chosen_port, sizeof(chosen_port)) != -1) {
                fprintf(stderr, "ERROR: Did not receive ephemeral port from child.\n");
                // handle error
            }
            printf("Parent got ephemeral port: %d\n", chosen_port);

            pipe_close(fd_read);
            // Now we can remove (unlink) the FIFO
            pipe_destroy(fifo_path);

            // Give the child a small time to fully listen (optional)
            sleep(1);

            // 4) Connect to the ephemeral port
            int socket_fd = connect_to_server("127.0.0.1", chosen_port);
            if (socket_fd < 0) {
                fprintf(stderr, "ERROR: Could not connect to server on port %d.\n", chosen_port);
                return 1;
            }

            // *** Existing "client main loop" from your code ***
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
