#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>

typedef struct simulation {
  int world_x;
  int world_y;
  int replications;
  double probabilities[4];
  int max_steps;
  char * file_to_save;
} simulation;

simulation init_simulation_parameters() {
  simulation sim;

  printf("Zadaj x a y rozmery sveta: \n");
  scanf(" %d %d", &sim.world_x, &sim.world_y);

  printf("Zadaj pocet replikacii: \n");
  scanf(" %d", &sim.replications);

  printf("Zadaj pravdepodobnosti posunov chodca, \n");
  printf("v poradi: HORE, DOLE, VLAVO, VPRAVO \n");
  scanf(" %lf %lf %lf %lf", &sim.probabilities[0], &sim.probabilities[1], &sim.probabilities[2], &sim.probabilities[3]);
  //todo check if the sum is 1

  printf("Zadaj maximalny pocet krokov chodca: \n");
  scanf( " %zu", &sim.max_steps);

  printf("Zadaj meno subora, kde sa ulozi vysledok: \n");
  scanf(" %99s", &sim.file_to_save);

  //todo add interactive_world, obstacle_world, file_to_load

  return sim;
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

int main(int argc, char *argv[])
{
  char input;

  print_ascii_menu_screen();
  scanf(" %c", &input);

  if (input == 'A' || input == 'a') {
    // todo init simulacie
    simulation sim = init_simulation_parameters();
    /* simulation_init(
     &sim.world_x, &sim.world_y, &sim.replications, &sim.probabilities,
     &sim.max_steps, &sim.file_to_save,
     bool interactive_world, bool obstacle_world, char *file_to_load);
    */

  } else if (input == 'B' || input == 'b') {
    // todo pripojenie k existujucej simulacii
  } else if (input == 'C' || input == 'c') {
    // todo opatovne spustenie simulacie
  } else if (input == 'D' || input == 'd') {
    exit(0);
  }

  return 0;
}