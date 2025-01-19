#include "client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../sockets-lib/socket.h"
#include "../server/server.h"

// int initialize_connection() {
//     if (client_socket != -1) {
//         return client_socket;
//     }
//
//     client_socket = connect_to_server(SERVER_ADDRESS, SERVER_PORT);
//     if (client_socket < 0) {
//         fprintf(stderr, "Zlyhalo pripojenie k serveru.\n");
//         return -1;
//     }
//     printf("Uspesne pripojeny k socketu %d.\n", client_socket);
//     return client_socket;
// }
int client_socket = -1;

void send_to_server(const char *message) {
    if (client_socket < 0) {
        fprintf(stderr, "Neplatny socket. Volal si initialize_connection()?\n");
        return;
    }

    if (write(client_socket, message, strlen(message)) < 0) {
        perror("Zlyhalo posielanie dat na server");
        return;
    }

    printf("Sprava odoslana na server uspesne.\n");
}

int receive_from_server(int socket_fd, char *buffer, int buffer_size) {
    memset(buffer, 0, buffer_size);
    int bytes_received = read(socket_fd, buffer, buffer_size - 1);

    if (bytes_received <= 0) {
        perror("Zlyhalo prijatie spravy zo servera");
        return -1;
    }

    printf("[SERVER->CLIENT]%s\n", buffer);
    return bytes_received;
}



void close_connection() {
    if (client_socket >= 0) {
        printf("Zatvaram socket %d.\n", client_socket);
        active_socket_destroy(client_socket);
        client_socket = -1;
    }
}
