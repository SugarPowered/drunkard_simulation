#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>

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
  } else if (input == 'B' || input == 'b') {
    // todo pripojenie k existujucej simulacii
  } else if (input == 'C' || input == 'c') {
    // todo opatovne spustenie simulacie
  } else if (input == 'D' || input == 'd') {
    exit(0);
  }

  return 0;
}