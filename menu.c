#include <stdio.h>
#include <stdlib.h>

#include "menu.h"

#include <math.h>

void input_direction_probability(simulation *sim) {

  printf("Zadaj pravdepodobnosti posunov chodca, \n");
  printf("v poradi: HORE, DOLE, VLAVO, VPRAVO \n");

  double checksum = 0.0;
  for (int i = 0; i < 4; i++) {
    scanf("%lf", &sim->probabilities[i]);
    checksum +=sim->probabilities[i];
  }

  if (round(checksum) == 1.0) { //todo upravit citlivost zaokruhlenia
    printf("Pravdepodobnosti zaznamenane.\n");
  } else {
    printf("Suma pravdepodobnosti musi byt 1! Skus znova.\n");
    input_direction_probability(sim);
  }
}

void initialize_simulation(simulation *sim) {

  printf("Zadaj x a y rozmery sveta: \n");
  scanf(" %d %d", &sim->world_x, &sim->world_y);

  printf("Zadaj pocet replikacii: \n");
  scanf(" %d", &sim->replications);

  input_direction_probability(sim);

  printf("Zadaj maximalny pocet krokov chodca: \n");
  scanf( " %zu", &sim->max_steps);

  printf("Zadaj meno subora, kde sa ulozi vysledok: \n");
  scanf(" %99s", &sim->file_to_save);

  printf("Je svet interaktivny? (1 = ano, 0 = nie): \n");
  scanf(" %d", &sim->interactive_world);

  printf("Obsahuje svet prekazky? (1 = ano, 0 = nie): \n");
  scanf(" %d", &sim->obstacle_world);

  printf("Zadaj meno subora na nacitanie sveta (alebo 'none'): \n");
  sim->file_to_load = calloc(100, sizeof(char));
  scanf(" %99s", sim->file_to_load);

}

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

void start_interactive_mode(simulation *sim) {
  printf("\nSpustam interaktivny mod...\n");
  //todo draw_simulation(sim);
  exit(0);
}

void start_summary_mode(simulation *sim) {
  printf("\nSpustam sumarny mod...\n");
  for (int i = 0; i < sim->replications; i++) {
    //todo step_simulation(sim);
  }
  //todo save_to_file(sim, sim->file_to_save);
  exit(0);
}

void menu() {
  char input;

  while (1) {
    print_ascii_menu_screen();
    scanf(" %c", &input);

    if (input == 'A' || input == 'a') {
      simulation sim;
      initialize_simulation(&sim);

      if (sim.interactive_world) {
        start_interactive_mode(&sim);
      } else {
        start_summary_mode(&sim);
      }
    } else if (input == 'B' || input == 'b') {
      // todo pripojenie k existujucej simulacii
    } else if (input == 'C' || input == 'c') {
      // todo opatovne spustenie simulacie
    } else if (input == 'D' || input == 'd') {
      printf("Koniec programu\n");
      break;
    } else {
      printf("Neplatna moznost, skus znova.\n");
    }
  }
}

// TEST funkcnosti
// int main(int argc, char *argv[]) {
//   menu();
//   return 0;
// }