#include "game_tools.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/* ********** GAME_LOAD ********** */

square char_to_square(char c) {
  if (c == 'e')
    return S_EMPTY;
  else if (c == 'w')
    return S_ZERO;
  else if (c == 'b')
    return S_ONE;
  else if (c == 'W')
    return S_IMMUTABLE_ZERO;
  else if (c == 'B')
    return S_IMMUTABLE_ONE;
  else {
    fprintf(stderr, "Err: invalid character in file\n");
    return EXIT_FAILURE;
  }
}

game game_load(char* filename) {
  FILE* f = fopen(filename, "r");
  if (f == NULL) return NULL;

  int nb_rows, nb_cols, wrapping, unique;
  int test = fscanf(f, "%d %d %d %d\n", &nb_rows, &nb_cols, &wrapping, &unique);
  if (test == 0) {
    fprintf(stderr, "Err: invalid file\n");
    return NULL;
  }

  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, unique);
  if (g == NULL) fprintf(stderr, "Null pointer\n");

  for (int i = 0; i < nb_rows; i++) {
    char line[nb_cols];
    char* t = fgets(line, nb_cols + 2, f);
    if (t == NULL) {
      fprintf(stderr, "Err: invalid file\n");
      return NULL;
    }
    for (uint j = 0; j < nb_cols; j++) {
      square s = char_to_square(line[j]);
      game_set_square(g, i, j, s);
    }
  }

  fclose(f);
  return g;
}

/* ********** GAME_SAVE ********** */

// converts square to its corresponding character
char square_to_char(square s) {
  if (s == S_EMPTY) return 'e';
  if (s == S_ZERO) return 'w';
  if (s == S_ONE) return 'b';
  if (s == S_IMMUTABLE_ZERO) return 'W';
  return 'B';
}

void game_save(cgame g, char* filename) {
  if (g == NULL) {
    fprintf(stderr, "Null pointer\n");
  }
  FILE* f = fopen(filename, "w");
  if (f == NULL) {
    fprintf(stderr, "Err: file could not be created\n");
  }

  // 1st line of the file
  fputc(game_nb_rows(g) + '0', f);
  fputc(32, f);
  fputc(game_nb_cols(g) + '0', f);
  fputc(32, f);
  fputc(game_is_wrapping(g) + '0', f);
  fputc(32, f);
  fputc(game_is_unique(g) + '0', f);
  fputc(10, f);

  // grid
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      fputc((int)square_to_char(game_get_square(g, i, j)), f);
    }
    fputc(10, f);
  }
  fclose(f);
}

/* ********** GAME_NB_SOLUTION ********** */
uint game_nb_solutions_aux(game g, int i, int j, int nb_rows, int nb_cols) {
  square s = game_get_square(g, i, j);
  int cpt = 0;
  if (i == nb_rows - 1 && j == nb_cols - 1) {
    if (s == S_IMMUTABLE_ONE || s == S_IMMUTABLE_ZERO) {
      if (!game_has_error(g, i, j)) {
        cpt += 1;
      }
    } else {
      game_set_square(g, i, j, S_ZERO);
      if (!game_has_error(g, i, j)) {
        cpt += 1;
      } else {
        game_set_square(g, i, j, S_ONE);
        if (!game_has_error(g, i, j)) {
          cpt += 1;
        }
      }
      game_set_square(g, i, j, S_EMPTY);
    }
    return cpt;
  }
  if (s == S_IMMUTABLE_ONE || s == S_IMMUTABLE_ZERO) {
    if (j == nb_cols - 1) {
      cpt += game_nb_solutions_aux(g, i + 1, 0, nb_rows, nb_cols);
    } else {
      cpt += game_nb_solutions_aux(g, i, j + 1, nb_rows, nb_cols);
    }

  } else {
    game_set_square(g, i, j, S_ZERO);
    if (!game_has_error(g, i, j)) {
      if (j == nb_cols - 1) {
        cpt += game_nb_solutions_aux(g, i + 1, 0, nb_rows, nb_cols);
      } else {
        cpt += game_nb_solutions_aux(g, i, j + 1, nb_rows, nb_cols);
      }
    }
    game_set_square(g, i, j, S_ONE);
    if (!game_has_error(g, i, j)) {
      if (j == nb_cols - 1) {
        cpt += game_nb_solutions_aux(g, i + 1, 0, nb_rows, nb_cols);
      } else {
        cpt += game_nb_solutions_aux(g, i, j + 1, nb_rows, nb_cols);
      }
    }
    game_set_square(g, i, j, S_EMPTY);
  }
  return cpt;
}

uint game_nb_solutions(cgame gPass) {
  game g = game_copy(gPass);
  uint cpt = game_nb_solutions_aux(g, 0, 0, game_nb_rows(g), game_nb_cols(g));
  game_delete(g);
  return cpt;
}

/* ********** GAME_SOLVE ********** */
void game_nb_solutions_forward(int* i, int* j, int nb_rows, int nb_cols) {
  if (*j == nb_cols - 1 && *i == nb_rows - 1) return;
  if (*j == nb_cols - 1 && *i <= nb_rows) {
    *i = *i + 1;
    *j = 0;
  } else {
    *j = *j + 1;
  }
}

void game_nb_solutions_backward(int* i, int* j, int nb_cols) {
  if (*i == 0 && *j == 0) return;
  if (*j <= 0 && *i >= 0) {
    *i = *i - 1;
    *j = nb_cols - 1;
  } else {
    *j = *j - 1;
  }
}

uint game_solve_aux(game g, bool one_sol) {
  game_restart(g);
  int i = 0;
  int j = 0;
  int cpt = 0;
  bool forward = true;
  int nb_rows = game_nb_rows(g);
  int nb_cols = game_nb_cols(g);
  square s = game_get_square(g, i, j);

  while (i != 0 || j != 0 || forward || (s != S_ONE && s != S_IMMUTABLE_ONE && s != S_IMMUTABLE_ZERO)) {
    if (i == nb_rows - 1 && j == nb_cols - 1 && s != S_EMPTY && !game_has_error(g, i, j)) {
      // Nous avons une solution, si on appelle depuis solve on arrête
      cpt += 1;
      if (one_sol) return cpt;
      game_set_square(g, i, j, S_EMPTY);
      game_nb_solutions_backward(&i, &j, nb_cols);
      forward = false;

    } else if (s == S_IMMUTABLE_ONE || s == S_IMMUTABLE_ZERO) {
      if (forward) {
        game_nb_solutions_forward(&i, &j, nb_rows, nb_cols);
      } else {
        game_nb_solutions_backward(&i, &j, nb_cols);
      }

    } else if (s == S_EMPTY) {
      game_set_square(g, i, j, S_ZERO);
      if (!game_has_error(g, i, j)) {
        game_nb_solutions_forward(&i, &j, nb_rows, nb_cols);
      }

    } else if (s == S_ZERO) {
      game_set_square(g, i, j, S_ONE);
      if (!game_has_error(g, i, j)) {
        game_nb_solutions_forward(&i, &j, nb_rows, nb_cols);
      }
      forward = true;

    } else if (s == S_ONE) {
      game_set_square(g, i, j, S_EMPTY);
      game_nb_solutions_backward(&i, &j, nb_cols);
      forward = false;
    }
    s = game_get_square(g, i, j);
  }
  return cpt;
}

bool game_solve(game g) { return game_solve_aux(g, true); }

/* ************************************************************************** */
/*                            Random Game Generator                           */
/* ************************************************************************** */

static game game_set_some_mutable_squares(game g, uint nb_empty)
{
  assert(g);
  uint nb_rows = game_nb_rows(g);
  uint nb_cols = game_nb_cols(g);
  uint nb_squares = nb_rows * nb_cols;
  assert(nb_empty <= nb_squares);
  uint nb_empty1 = nb_empty;
  while (nb_empty1 > 0) {
    uint pos = rand() % nb_squares;
    uint row = pos / nb_cols;
    uint col = pos % nb_cols;
    if (game_is_immutable(g, row, col)) {
      square s = game_get_number(g, row, col);
      game_set_square(g, row, col, s + 1);
      nb_empty1--;
    }
  }
  return g;
}

/* ************************************************************************** */

static void game_flip_one_square(game g, int i, int j)
{
  square s = game_get_square(g, i, j);
  if (s == S_IMMUTABLE_ONE)
    game_set_square(g, i, j, S_IMMUTABLE_ZERO);
  else
    game_set_square(g, i, j, S_IMMUTABLE_ONE);
}

/* ************************************************************************** */

static bool try_flip(game g, int i1, int j1, int i2, int j2)
{
  if (i1 == i2 && j1 == j2) return false;
  assert(game_is_over(g));
  square s11 = game_get_square(g, i1, j1);
  square s22 = game_get_square(g, i2, j2);
  square s12 = game_get_square(g, i1, j2);
  square s21 = game_get_square(g, i2, j1);

  if (s11 == s22 && s12 == s21 && s11 != s12) {
    game_flip_one_square(g, i1, j1);
    game_flip_one_square(g, i2, j2);
    game_flip_one_square(g, i1, j2);
    game_flip_one_square(g, i2, j1);

    if (game_is_over(g)) {
      return true;
    } else {  // undo
      game_flip_one_square(g, i1, j1);
      game_flip_one_square(g, i2, j2);
      game_flip_one_square(g, i1, j2);
      game_flip_one_square(g, i2, j1);
      assert(game_is_over(g));
      return false;
    }
  }
  return false;
}

/* ************************************************************************** */

static void apply_nb_flips(game g, uint nb)
{
  uint nb_rows = game_nb_rows(g);
  uint nb_cols = game_nb_cols(g);
  while (nb > 0) {
    uint i1 = rand() % nb_rows;
    uint j1 = rand() % nb_cols;
    uint i2 = rand() % nb_rows;
    uint j2 = rand() % nb_cols;
    if (try_flip(g, i1, j1, i2, j2)) nb--;
  }
}

/* ************************************************************************** */

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint uniq, bool with_solution)
{
  // step 0: create an empty game
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, uniq);

  // fill the grid with initial values
  for (uint i = 0; i < nb_rows; i++)
    for (uint j = 0; j < nb_cols; j++) game_set_square(g, i, j, (i + j) % 2 + 3);

  // Make it unique if it is square
  if (nb_cols == nb_rows) {
    for (uint i = 0; i < nb_rows / 2; i++) {
      game_flip_one_square(g, 2 * i, 2 * i);
      game_flip_one_square(g, 2 * i, 2 * i + 1);
      game_flip_one_square(g, 2 * i + 1, 2 * i);
      game_flip_one_square(g, 2 * i + 1, 2 * i + 1);
    }
  }
  assert(game_is_over(g));
  // apply some flips
  apply_nb_flips(g, nb_rows * nb_cols * 10);

  assert(game_is_over(g));
  // check

  game_set_some_mutable_squares(g, (nb_rows * nb_cols * 1) / 2);
  if (!with_solution) game_restart(g);
  return g;
}

/* ************************************************************************** */