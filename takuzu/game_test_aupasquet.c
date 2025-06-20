#define __USE_GNU

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

/* ***************** ASSERT ***************** */

#define ASSERT(expr)                                                                  \
  do {                                                                                \
    if ((expr) == 0) {                                                                \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, #expr); \
      abort();                                                                        \
    }                                                                                 \
  } while (0)

/* ***************** TEST_GAME_DEFAULT ***************** */

bool test_game_default() {
  game g = game_default();
  ASSERT(g);

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      if ((i == 0 && j == 1) || ((i == 3 || i == 4) && j == 2)) {
        ASSERT(game_get_square(g, i, j) == S_IMMUTABLE_ONE);
      } else if (((i == 2 || i == 3) && j == 1) || (i == 0 && j == 2) || (i == 2 && j == 4) ||
                 ((i == 4 || i == 5) && j == 5)) {
        ASSERT(game_get_square(g, i, j) == S_IMMUTABLE_ZERO);
      } else {
        ASSERT(game_get_square(g, i, j) == S_EMPTY);
      }
    }
  }

  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_RESTART ***************** */

// checks : all the squares are empty, except the values of the last line (which are immutable_zero)
void verify_grid_values(game g) {
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (i == 5) {
        ASSERT(game_get_square(g, i, j) == S_IMMUTABLE_ZERO);
      } else {
        ASSERT(game_is_empty(g, i, j));
      }
    }
  }
}

bool test_game_restart() {
  game g1 = game_default();
  ASSERT(g1);
  game g2 = game_default_solution();
  ASSERT(g2);
  game_restart(g2);
  ASSERT(game_equal(g1, g2));
  game_delete(g1);
  game_delete(g2);

  game g = game_new_empty_ext(6, 6, true, true);
  ASSERT(g);

  for (int j = 0; j < game_nb_rows(g); j++) {
    game_set_square(g, 5, j, S_IMMUTABLE_ZERO);
  }

  for (int i = 0; i < game_nb_cols(g) - 1; i++) {
    game_play_move(g, i, i, S_ONE);
  }

  game_restart(g);

  game_undo(g);
  verify_grid_values(g);

  game_redo(g);
  verify_grid_values(g);

  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_CHECK_MOVE ***************** */

// checks that it's illegal to play on an immutable square, and legal to play on a regular square
void check_square(game g, uint i, uint j, square s1[], uint n1, square s2[], uint n2) {
  for (int k = 0; k < n1; k++) {
    game_set_square(g, i, j, s1[k]);

    for (int l = 0; l < n1; l++) {
      ASSERT(game_check_move(g, i, j, s1[l]));
    }
  }

  for (int l = 0; l < n2; l++) {
    for (int k = 0; k < n1; k++) {
      game_set_square(g, i, j, s2[l]);
      ASSERT(!game_check_move(g, i, j, s1[k]));
    }
  }
}

bool test_game_check_move() {
  game g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true, true);
  ASSERT(g);
  square s1[] = {S_EMPTY, S_ZERO, S_ONE};
  uint n1 = 3;
  square s2[] = {S_IMMUTABLE_ZERO, S_IMMUTABLE_ONE};
  uint n2 = 2;

  for (int i = 0; i < n1; i++) {
    ASSERT(!game_check_move(g, DEFAULT_SIZE, DEFAULT_SIZE, s1[i]));
  }

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      check_square(g, i, j, s1, n1, s2, n2);
    }
  }

  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_IS_EMPTY ***************** */

bool test_game_is_empty() {
  game g = game_new_empty();
  ASSERT(g);

  square s_tab[] = {S_ZERO, S_ONE, S_IMMUTABLE_ZERO, S_IMMUTABLE_ONE};
  uint n_tab = 4;

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      ASSERT(game_is_empty(g, i, j));

      for (int k = 0; k < n_tab; k++) {
        game_set_square(g, i, j, s_tab[k]);
        ASSERT(!game_is_empty(g, i, j));
      }
    }
  }

  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_GET_NUMBER ***************** */

bool test_game_get_number() {
  game g = game_new_empty();
  ASSERT(g);

  square s_tab[] = {S_ZERO, S_ONE, S_IMMUTABLE_ZERO, S_IMMUTABLE_ONE};
  uint n_tab = 4;
  int t_possibilities[] = {0, 1, 0, 1};

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      ASSERT(game_get_number(g, i, j) == -1);
      for (int k = 0; k < n_tab; k++) {
        game_set_square(g, i, j, s_tab[k]);
        ASSERT(game_get_number(g, i, j) == t_possibilities[k]);
      }
    }
  }

  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_DELETE ***************** */

bool test_game_delete() {
  game g = game_new_empty();
  ASSERT(g);
  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_NEW_EMPTY ***************** */

bool test_game_new_empty() {
  game g = game_new_empty();
  ASSERT(g);

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      ASSERT(game_is_empty(g, i, j));
    }
  }

  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_NB_ROWS ***************** */

bool test_game_nb_rows() {
  game g = game_default();
  ASSERT(game_nb_rows(g) == DEFAULT_SIZE);
  game_delete(g);
  game g1 = game_new_empty_ext(8, 8, false, false);
  ASSERT(game_nb_rows(g1) == 8);
  game_delete(g1);
  return true;
}

/* ***************** TEST_GAME_NB_COLS ***************** */

bool test_game_nb_cols() {
  game g = game_default();
  ASSERT(game_nb_cols(g) == DEFAULT_SIZE);
  game_delete(g);
  game g1 = game_new_empty_ext(8, 8, false, false);
  ASSERT(game_nb_cols(g1) == 8);
  game_delete(g1);
  return true;
}

/* ***************** TEST_GAME_NEW_EXT ***************** */

bool test_game_new_ext() {
  square s[] = {S_EMPTY, S_IMMUTABLE_ONE, S_EMPTY, S_EMPTY, S_IMMUTABLE_ZERO, S_EMPTY,
                S_EMPTY, S_IMMUTABLE_ONE, S_EMPTY, S_EMPTY, S_IMMUTABLE_ZERO, S_EMPTY,
                S_EMPTY, S_IMMUTABLE_ONE, S_EMPTY, S_EMPTY, S_IMMUTABLE_ZERO, S_EMPTY,

                S_EMPTY, S_IMMUTABLE_ONE, S_EMPTY, S_EMPTY, S_IMMUTABLE_ZERO, S_EMPTY,
                S_EMPTY, S_IMMUTABLE_ONE, S_EMPTY, S_EMPTY, S_IMMUTABLE_ZERO, S_EMPTY};

  game g = game_new_ext(4, 6, s, false, true);
  ASSERT(g);

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      ASSERT(game_get_square(g, i, j) == s[i * game_nb_cols(g) + j]);
    }
  }

  ASSERT(game_nb_rows(g) == 4);
  ASSERT(game_nb_cols(g) == 6);
  ASSERT(!game_is_wrapping(g));
  ASSERT(game_is_unique(g));

  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_NEW_EMPTY_EXT ***************** */

bool test_game_new_empty_ext() {
  game g1 = game_new_empty_ext(4, 6, true, true);
  ASSERT(g1);

  ASSERT(game_nb_rows(g1) == 4);
  ASSERT(game_nb_cols(g1) == 6);
  ASSERT(game_is_wrapping(g1));
  ASSERT(game_is_unique(g1));

  for (uint i = 0; i < game_nb_rows(g1); i++) {
    for (uint j = 0; j < game_nb_cols(g1); j++) {
      ASSERT(game_get_square(g1, i, j) == S_EMPTY);
    }
  }

  game_delete(g1);

  return true;
}

/* ***************** TEST_GAME_UNDO ***************** */

bool test_game_undo() {
  game g = game_new_empty_ext(2, 2, false, false);
  game_play_move(g, 0, 0, S_ONE);
  game_play_move(g, 1, 0, S_ONE);
  game_play_move(g, 1, 0, S_ZERO);
  game_play_move(g, 0, 0, S_ZERO);
  game_undo(g);
  ASSERT(game_get_square(g, 0, 0) == S_ONE);
  game_undo(g);
  ASSERT(game_get_square(g, 1, 0) == S_ONE);
  game_delete(g);
  return true;
}

/* ***************** TEST_GAME_REDO ***************** */

bool test_game_redo() {
  game g = game_new_empty_ext(2, 2, true, false);
  game_set_square(g, 0, 1, S_IMMUTABLE_ONE);

  game_play_move(g, 0, 0, S_ONE);
  game_play_move(g, 0, 0, S_ZERO);
  game_undo(g);
  game_redo(g);
  ASSERT(game_get_square(g, 0, 0) == S_ZERO);
  game_undo(g);
  ASSERT(game_get_square(g, 0, 0) == S_ONE);

  game_delete(g);
  return true;
}

/* ***************** TEST_DUMMY ***************** */

bool test_dummy() { return true; }

/* ***************** USAGE ***************** */

void usage(int argc, char* argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ***************** MAIN ***************** */

int main(int argc, char* argv[]) {
  if (argc == 1) usage(argc, argv);

  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool ok = false;

  // Create a struct array with test function pointers and their corresponding test names
  struct test_data {
    char* name;
    bool (*test)();
  } tests[] = {{"dummy", test_dummy},
               {"game_new_empty", test_game_new_empty},
               {"game_delete", test_game_delete},
               {"game_get_number", test_game_get_number},
               {"game_is_empty", test_game_is_empty},
               {"game_check_move", test_game_check_move},
               {"game_restart", test_game_restart},
               {"game_default", test_game_default},
               {"game_nb_rows", test_game_nb_rows},
               {"game_nb_cols", test_game_nb_cols},
               {"game_new_empty_ext", test_game_new_empty_ext},
               {"game_new_ext", test_game_new_ext},
               {"game_undo", test_game_undo},
               {"game_redo", test_game_redo}};

  // Iterate through the struct array and find the test function that matches the name provided as an argument
  for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    if (strcmp(tests[i].name, argv[1]) == 0) {
      ok = tests[i].test();
      break;
    }
  }
  if (!ok) {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  // print test result
  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}