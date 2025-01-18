#include "client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../sockets-lib/socket.h"
#include "../server/server.h"

static int client_socket = -1;

int send_message(int socket, message_type_t type, const char *data) {
    protocol_message_t msg;
    msg.type = type;
    msg.length = strlen(data);
    strncpy(msg.data, data, MSG_MAX_SIZE - 1);

    return write(socket, &msg, sizeof(protocol_message_t));
}

int receive_message(int socket, protocol_message_t *msg) {
    int bytes = read(socket, msg, sizeof(protocol_message_t));
    if (bytes <= 0) return bytes;

    msg->data[msg->length] = '\0';
    return bytes;
}

void send_to_server(const char *message) {
    if (client_socket < 0) {
        fprintf(stderr, "Neplatny socket. Volal si initialize_connection()?\n");
        return;
    }

    if (send_message(client_socket, MSG_START_SIM, message) < 0) {
        perror("Zlyhalo posielanie dat na server");
        return;
    }

    printf("Sprava odoslana na server uspesne.\n");
}

int receive_from_server(int socket_fd, char *buffer, int buffer_size) {
    protocol_message_t msg;
    int bytes_received = receive_message(socket_fd, &msg);

    if (bytes_received <= 0) {
        perror("Zlyhalo prijatie spravy zo servera");
        return -1;
    }

    switch (msg.type) {
      	case MSG_WELCOME:
          	printf("%s", msg.data);
            break;
        case MSG_SIM_COMPLETE:
            strncpy(buffer, msg.data, buffer_size - 1);
            printf("Simulacia dokoncena.\n");
            break;

        case MSG_SIM_UPDATE:
            strncpy(buffer, msg.data, buffer_size - 1);
            update_world_from_server(msg.data);
            break;

        case MSG_ERROR:
            fprintf(stderr, "Chyba zo servera: %s\n", msg.data);
            return -1;

        default:
            fprintf(stderr, "Neznamy typ spravy: %d\n", msg.type);
            return -1;
    }

    return bytes_received;
}

int initialize_connection() {
    if (client_socket != -1) {
        return client_socket;
    }

    client_socket = connect_to_server(SERVER_ADDRESS, SERVER_PORT);
    if (client_socket < 0) {
        fprintf(stderr, "Zlyhalo pripojenie k serveru.\n");
        return -1;
    }
    printf("Uspesne pripojeny k socketu %d.\n", client_socket);
    return client_socket;
}

void close_connection() {
    if (client_socket >= 0) {
        printf("Zatvaram socket %d.\n", client_socket);
        active_socket_destroy(client_socket);
        client_socket = -1;
    }
}
