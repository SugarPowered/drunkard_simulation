//
// Created by michal.subert on 10. 1. 2025.
//

#include "client.h"

void client_connect(Client *client, const char *ip, int port) {
    client->socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // todo najdi WIN alt
    //inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    if (connect(client->socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return;
    }

    receive_data(client); // todo start receiving data from the server
}

void receive_data(Client *client) {
    // todo handle receiving data from the server and rendering it.
}