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

void handle_server_message(const char *msg) {
    // Example message: "INFO|MENU"
    char copy[BUFFER_SIZE];
    strncpy(copy, msg, sizeof(copy));
    copy[sizeof(copy) - 1] = '\0';

    // First token might be "INFO"
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

        printf("[CLIENT->SERVER] Prijal som data replikacie, updatujem svet...\n");
        update_world_from_server(token);
        render_world();
    } else {
        //printf("[SERVER->CLIENT] %s\n", msg);
    }
}

// Prompts user (H)ost or (J)oin?
static int ask_user_for_mode() {
    char choice;
    printf("\nChcete vytvorit novy server (H) alebo sa pripojit k existujucemu serveru (J)?: ");
    scanf(" %c", &choice);
    if (choice == 'H' || choice == 'h') return 1;
    return 0;
}

void sim_loop(int socket_fd) {
    char buffer[BUFFER_SIZE];

    clear_world(); // Initialize the world state
    render_world();

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = receive_from_server(socket_fd, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            printf("Server sa odpojil alebo doslo k chybe.\n");
            break;
        }

        handle_server_message(buffer);
    }
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

        printf("Vytvaram novy pipe %s\n", pipe);

        pipe_init(pipe);

        pid_t cpid = fork();
        if (cpid < 0) {
            perror("Nepodarilo sa vytvorit fork server\n");
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
            printf("Spustenie klienta za 1 sekundu...\n");
            sleep(1);
            int fd_read = pipe_open_read(pipe);

            if (read(fd_read, &chosen_port, sizeof(chosen_port)) == -1) {
                fprintf(stderr, "CHYBA: Nepodarilo sa prijimat efemerny port od dietata.\n");
                exit(EXIT_FAILURE);
            }
            printf("Rodic dostal efemerny port: %d\n", chosen_port);

            pipe_close(fd_read);
            pipe_destroy(pipe);

            sleep(1);

            int socket_fd = connect_to_server("127.0.0.1", chosen_port);
            if (socket_fd < 0) {
                fprintf(stderr, "CHYBA: Nepodarilo sa pripojit k serveru na porte %d.\n", chosen_port);
                return 1;
            }
            client_socket = socket_fd;

            sim_loop(socket_fd);

            close_connection();
        }
    }
    else {
        printf("Zadajte port servera na pripojenie: ");
        scanf("%d", &chosen_port);

        int socket_fd = connect_to_server("127.0.0.1", chosen_port);
        if (socket_fd < 0) {
            fprintf(stderr, "CHYBA: Nepodarilo sa pripojit k serveru na porte %d.\n", chosen_port);
            return 1;
        }
        client_socket = socket_fd;

        sim_loop(socket_fd);

        close_connection();
    }
    return 0;
}
