#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "menu.h"
#include "../sockets-lib/socket.h"

void print_ascii_menu_screen() {
    printf("==========================================================\n");
    printf("          __  _               _         __               \n");
    printf("         /_/ | |             | |       /_/               \n");
    printf("  _ __   __ _| |__   ___   __| |_ __   __ _              \n");
    printf(" | '_ \\ / _` | '_ \\ / _ \\ / _` | '_ \\ / _` |             \n");
    printf(" | | | | (_| | | | | (_) | (_| | | | | (_| |             \n");
    printf(" |_| |_|\\__,_|_| |_|\\___/ \\__,_|_| |_|\\__,_|             \n");
    printf("                   _      //\\      _     _              \n");
    printf("                  | |    |/ \\|    | |   | |             \n");
    printf("  _ __   ___   ___| |__   ___   __| |___| | ____ _      \n");
    printf(" | '_ \\ / _ \\ / __| '_ \\ / _ \\ / _` |_  / |/ / _` |      \n");
    printf(" | |_) | (_) | (__| | | | (_) | (_| |/ /|   < (_| |      \n");
    printf(" | .__/ \\___/ \\___|_| |_|\\___/ \\__,_/___|_|\\_\\__,_|      \n");
    printf(" | |                                                    \n");
    printf(" |_|         by **Denis Uhrik & Michal Šubert**         \n");
    printf("==========================================================\n \n");
    printf("~~ Vita vas simulacia nahodnej pochodzky. ~~\n");
    printf("Zvolte si jednu z moznosti a napiste prislusne pismeno do terminalu.\n\n");
    printf("[A] Nova simulacia \n");
    printf("[B] Pripojenie k simulacii \n");
    printf("[C] Opatovne spustenie simulacie \n");
    printf("[D] Koniec programu \n");
}

void get_simulation_parameters() {
    int world_width, world_height, replications, max_steps;
    double probabilities[4], sum = 0.0;
    char output_file[100], buffer[1024];

    printf("\nEnter the dimensions of the world (width height): ");
    scanf("%d %d", &world_width, &world_height);

    printf("\nEnter the number of replications: ");
    scanf("%d", &replications);

    printf("\nEnter the probabilities for movement (up, down, left, right): ");
    for (int i = 0; i < 4; i++) {
        scanf("%lf", &probabilities[i]);
        sum += probabilities[i];
    }

    if (sum != 1.0) {
        printf("\nError: Probabilities must sum to 1.0.\n");
        return;
    }

    printf("\nEnter the maximum number of steps (K): ");
    scanf("%d", &max_steps);

    printf("\nEnter the output file name: ");
    scanf("%s", output_file);

    snprintf(buffer, sizeof(buffer), "START_SIMULATION;%d;%d;%d;%.2f,%.2f,%.2f,%.2f;%d;%s",
             world_width, world_height, replications,
             probabilities[0], probabilities[1], probabilities[2], probabilities[3],
             max_steps, output_file);

    send_to_server(buffer);

    printf("\nSimulation parameters sent to server.\n");
}

void get_replay_parameters() {
    char input_file[100], buffer[1024];

    printf("\nEnter the file name to load the replay from: ");
    scanf("%s", input_file);

    snprintf(buffer, sizeof(buffer), "REPLAY_SIMULATION;%s", input_file);

    send_to_server(buffer);

    printf("\nReplay parameters sent to server.\n");
}

void display_menu() {
    char input;

    while (1) {
        print_ascii_menu_screen();
        printf("Please select an option: ");
        scanf(" %c", &input);

        switch (input) {
            case 'A':
            case 'a':
                printf("\nStarting a new simulation...\n");
                get_simulation_parameters();
                break;

            case 'B':
            case 'b':
                printf("\nJoining an existing simulation...\n");
                // Call function to join an existing simulation
                break;

            case 'C':
            case 'c':
                printf("\nReplaying a previous simulation...\n");
                get_replay_parameters();
                break;

            case 'D':
            case 'd':
                printf("\nExiting the program. Goodbye!\n");
                exit(0);
                break;

            default:
                printf("\nInvalid selection, please try again.\n");
                break;
        }
    }
}

void initialize_menu() {
    display_menu();
}
