#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"

void game_print(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Null pointer\n");
  }
  printf("  ");
  for (int i = 0; i < game_nb_cols(g); i++) {
    printf("%d", i);
  }
  printf(" \n  ");
  for (int i = 0; i < game_nb_cols(g); i++) {
    printf("-");
  }
  printf(" \n");
  for (int i = 0; i < game_nb_rows(g); i++) {
    printf("%d|", i);
    for (int j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      if (s == S_EMPTY) {
        printf(" ");
      } else if (s == S_ZERO) {
        printf("w");
      } else if (s == S_ONE) {
        printf("b");
      } else if (s == S_IMMUTABLE_ONE) {
        printf("B");
      } else {
        printf("W");
      }
    }
    printf("|\n");
  }
  printf("  ");
  for (int i = 0; i < game_nb_cols(g); i++) {
    printf("-");
  }
  printf(" \n");
}

game game_default(void) {
  game g = game_new_empty();
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (((i == 2 || i == 3) && j == 1) || (i == 0 && j == 2) || (i == 2 && j == 4) ||
          ((i == 4 || i == 5) && j == 5)) {
        game_set_square(g, i, j, S_IMMUTABLE_ZERO);
      } else if ((i == 0 && j == 1) || ((i == 3 || i == 4) && j == 2)) {
        game_set_square(g, i, j, S_IMMUTABLE_ONE);
      }
    }
  }
  return g;
}

game game_default_solution(void) {
  game g = game_default();

  if (g == NULL) return NULL;

  // s == S_ZERO
  game_set_square(g, 0, 0, S_ZERO);
  game_set_square(g, 0, 4, S_ZERO);
  game_set_square(g, 1, 0, S_ZERO);
  game_set_square(g, 1, 3, S_ZERO);
  game_set_square(g, 1, 5, S_ZERO);
  game_set_square(g, 2, 2, S_ZERO);
  game_set_square(g, 3, 3, S_ZERO);
  game_set_square(g, 3, 4, S_ZERO);
  game_set_square(g, 4, 0, S_ZERO);
  game_set_square(g, 4, 3, S_ZERO);
  game_set_square(g, 5, 1, S_ZERO);
  game_set_square(g, 5, 2, S_ZERO);

  // s == S-ONE
  game_set_square(g, 0, 3, S_ONE);
  game_set_square(g, 0, 5, S_ONE);
  game_set_square(g, 1, 1, S_ONE);
  game_set_square(g, 1, 2, S_ONE);
  game_set_square(g, 1, 4, S_ONE);
  game_set_square(g, 2, 0, S_ONE);
  game_set_square(g, 2, 3, S_ONE);
  game_set_square(g, 2, 5, S_ONE);
  game_set_square(g, 3, 0, S_ONE);
  game_set_square(g, 3, 5, S_ONE);
  game_set_square(g, 4, 1, S_ONE);
  game_set_square(g, 4, 4, S_ONE);
  game_set_square(g, 5, 0, S_ONE);
  game_set_square(g, 5, 3, S_ONE);
  game_set_square(g, 5, 4, S_ONE);

  return g;
}