#include "client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../sockets-lib/socket.h"
#include "../server/server.h"

// Funkcia na odoslanie správy na server
void send_to_server(const char *message) {
    int sock = connect_to_server(SERVER_ADDRESS, SERVER_PORT);

    if (sock < 0) {
        fprintf(stderr, "Failed to connect to server.\n");
        return;
    }

    if (write(sock, message, strlen(message)) < 0) {
        perror("Error sending data to server");
        active_socket_destroy(sock);
        return;
    }

    printf("Message sent to server successfully.\n");

    char buffer[BUFFER_SIZE];
    int bytes_received = receive_from_server(sock, buffer, BUFFER_SIZE);
    if (bytes_received > 0) {
        printf("Received from server: %s\n", buffer);
    } else {
        perror("Error receiving data from server");
    }

    active_socket_destroy(sock); // tu sa socket zatvori posledny raz
}

// Funkcia na prijatie odpovede zo servera
int receive_from_server(int socket_fd, char *buffer, int buffer_size) {

  	int client_socket = socket_fd;
    memset(buffer, 0, buffer_size);
    int bytes_received = read(socket_fd, buffer, buffer_size - 1);

    if (bytes_received < 0) {
        perror("Failed to receive message from server");
        return -1;
    }
    printf("Message received from server: %s\n", buffer);

    return bytes_received;
}
