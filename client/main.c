//
// Created by michal.subert on 10. 1. 2025.
//

#include "client.h"

int main() {
    Client client;
    client_connect(&client, "localhost", PORT); //todo change the IP
    return 0;
}
