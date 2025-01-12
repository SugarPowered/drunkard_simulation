#include "input_handler.h"
#include "../sockets-lib/socket.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void send_user_input(int socket, const char *input) {
    if (write(socket, input, strlen(input)) < 0) {
        fprintf(stderr, "Failed to send input to server.\n");
    }
}