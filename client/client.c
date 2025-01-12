#include "client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../sockets-lib/socket.h"

// Funkcia na odoslanie správy na server
void send_to_server(int socket_fd, const char *message) {
    if (write(socket_fd, message, strlen(message)) == -1) {
        perror("Failed to send message to server");
    } else {
        printf("Message sent to server: %s\n", message);
    }
}

// Funkcia na prijatie odpovede zo servera
int receive_from_server(int socket_fd, char *buffer, int buffer_size) {
    memset(buffer, 0, buffer_size);
    int bytes_received = read(socket_fd, buffer, buffer_size - 1);
    if (bytes_received < 0) {
        perror("Failed to receive message from server");
        return -1;
    }
    printf("Message received from server: %s\n", buffer);
    return bytes_received;
}
