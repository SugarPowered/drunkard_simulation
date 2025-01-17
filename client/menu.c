#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "menu.h"

#include "client.h"
#include "../sockets-lib/socket.h"

int menu_rendered = 0;

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
    printf("[D] Koniec programu \n \n");
}

void get_simulation_parameters() {
    int world_width, world_height, replications, max_steps, has_obstacles, is_interactive;
    double probabilities[4], sum = 0.0;
    char output_file[100], buffer[1024];

    printf("\nVloz rozmery sveta (sirka vyska): ");
    scanf("%d %d", &world_width, &world_height);

    printf("\nVloz pocet replikacii: ");
    scanf("%d", &replications);

    printf("\nVloz pravdepodobnosti pohybu (hore, dole, dolava, doprava): ");
    for (int i = 0; i < 4; i++) {
        scanf("%lf", &probabilities[i]);
        sum += probabilities[i];
    }

    if (sum != 1.0) {
        printf("\nError: Pravdepodobnosti sa musia scitat do 1.0.\n");
        return;
    }

    printf("\nVloz maximalny pocet krokov (K): ");
    scanf("%d", &max_steps);

    printf("\nChces svet s prekazkami? 1 - ano, 0 - nie : ");
    scanf("%d", &has_obstacles);

    printf("\nAky chces mod? 1 - interaktivny, 0 - sumarny : ");
    scanf("%d", &is_interactive);

    printf("\nVloz nazov vystupneho suboru: ");
    scanf("%s", output_file);

    snprintf(buffer, sizeof(buffer), "START_SIMULATION;%d;%d;%d;%.2f,%.2f,%.2f,%.2f;%d;%d;%d;%s",
             world_width, world_height, replications,
             probabilities[0], probabilities[1], probabilities[2], probabilities[3],
             max_steps, has_obstacles,is_interactive, output_file);

    send_to_server(buffer);

    printf("Simulacne parametre odoslane na server.\n\n");
}

void get_replay_parameters() {
    char input_file[100], buffer[1024];

    printf("\nVloz nazov suboru na nacitanie: ");
    scanf("%s", input_file);

    snprintf(buffer, sizeof(buffer), "REPLAY_SIMULATION;%s", input_file);

    send_to_server(buffer);

    printf("\nParametre zopakovania simulacie odoslane na server.\n");
}

void display_menu() {
    char input;
    print_ascii_menu_screen();
    printf("Prosim zvol moznost: ");
    scanf(" %c", &input);
    while (menu_rendered == 0) {
        menu_rendered = 1;
        switch (input) {
            case 'A':
            case 'a':
                printf("\nZacinam novu simulaciu...\n");
                get_simulation_parameters();
                break;

            case 'B':
            case 'b':
                printf("\nPripajam sa k existujucej simulacii...\n");
                get_replay_parameters();
                // Call function to join an existing simulation
                break;

            case 'C':
            case 'c':
                printf("\nPrehravam predoslu simulaciu...\n");
                get_replay_parameters();
                break;

            case 'D':
            case 'd':
                printf("\nKoncim program, AHOJ!\n");
                exit(0);

            default:
                printf("\nNeplatna volba, skus to znova.\n");
                break;
        }
    }
}
