#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "menu.h"
#include "../sockets-lib/socket.h"

#define BUFFER_SIZE 2048

int main() {
    int socket_fd = initialize_connection();
    if (socket_fd < 0) {
        fprintf(stderr, "ERROR: Pripojenie ku serveru zlyhalo.\n");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));

        // 2. Wait for data from server
        int bytes_received = receive_from_server(socket_fd, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            printf("Server sa odpojil alebo nastala chyba.\n");
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
    return 0;
}
