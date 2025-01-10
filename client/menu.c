#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "menu.h"
#include "../server/simulation.h"

void start_menu() {
  char input;

  while (1) {
    print_ascii_menu_screen();
    scanf(" %c", &input);

    if (input == 'A' || input == 'a') {
      simulation sim;
      input_simulation_params(&sim);

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

// TEST if menu works
int main(int argc, char *argv[]) {
  start_menu();
  return 0;
}