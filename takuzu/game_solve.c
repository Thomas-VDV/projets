#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char* argv[]) {
  if (argc < 3 || argc > 4) {
    printf("Wrong number of argument\n");
    return EXIT_FAILURE;
  }
  game g = game_load(argv[2]);
  if (g == NULL) {
    printf("Error while loading the game\n");
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "-s") == 0) {
    if (game_solve(g)) {
      if (argc == 4)
        game_save(g, argv[3]);
      else
        game_print(g);
    } else {
      return EXIT_FAILURE;
    }

  } else if (strcmp(argv[1], "-c") == 0) {
    int nb = game_nb_solutions(g);
    if (argc == 3) {
      printf("%d\n", nb);
    } else {
      FILE* f = fopen(argv[3], "w");
      assert(f);
      char str[10];
      sprintf(str, "%d", nb);
      fputs(str, f);
      fclose(f);
    }
  }
}