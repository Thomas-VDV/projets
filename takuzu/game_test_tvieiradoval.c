#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/* ********** ASSERT ********** */

#define ASSERT(expr)                                                                  \
  do {                                                                                \
    if ((expr) == 0) {                                                                \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, #expr); \
      abort();                                                                        \
    }                                                                                 \
  } while (0)

/* ********** TEST GAME_NEW ********** */
bool test_game_new() {
  square t_square[] = {S_EMPTY, S_IMMUTABLE_ONE, S_IMMUTABLE_ZERO};

  square s[DEFAULT_SIZE][DEFAULT_SIZE];
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      s[i][j] = t_square[rand() % 3];
    }
  }

  game g = game_new(*s);
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      ASSERT(s[i][j] == game_get_square(g, i, j));
    }
  }
  game_delete(g);

  return true;
}

/* ********** TEST GAME_EQUAL ********** */
bool test_game_equal() {
  game g1 = game_default();
  game g2 = game_default();
  ASSERT(game_equal(g1, g2));

  game_set_square(g1, 0, 0, S_ZERO);
  game_set_square(g2, 0, 0, S_IMMUTABLE_ZERO);
  ASSERT(game_equal(g1, g2) == false);

  game_delete(g1);
  game_delete(g2);
  return true;
}

/* ********** TEST GAME_GET_SQUARE ********** */
bool test_game_get_square() {
  game g = game_new_empty();
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      game_set_square(g, i, j, S_IMMUTABLE_ONE);
      ASSERT(game_get_square(g, i, j) == S_IMMUTABLE_ONE);
      game_set_square(g, i, j, S_IMMUTABLE_ZERO);
      ASSERT(game_get_square(g, i, j) == S_IMMUTABLE_ZERO);
      game_set_square(g, i, j, S_ONE);
      ASSERT(game_get_square(g, i, j) == S_ONE);
      game_set_square(g, i, j, S_ZERO);
      ASSERT(game_get_square(g, i, j) == S_ZERO);
      game_set_square(g, i, j, S_EMPTY);
      ASSERT(game_get_square(g, i, j) == S_EMPTY);
    }
  }
  game_delete(g);
  return true;
}

/* ********** TEST GAME GET NEXT NUMBER********** */

bool test_game_get_next_number() {
  game g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true, true);
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (((i == 2 || i == 3) && j == 1) || (i == 0 && j == 2) || (i == 2 && j == 4) ||
          ((i == 4 || i == 5) && j == 5)) {
        game_set_square(g, i, j, S_IMMUTABLE_ZERO);
      } else if ((i == 0 && j == 1) || ((i == 3 || i == 4) && j == 2)) {
        game_set_square(g, i, j, S_IMMUTABLE_ONE);
      }
    }
  }

  ASSERT(g);
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      for (int dist = 1; dist <= 2; dist++) {
        if (i < dist) {
          ASSERT(game_get_next_number(g, i, j, UP, dist) == game_get_number(g, game_nb_rows(g) + i - dist, j));
        } else {
          ASSERT(game_get_next_number(g, i, j, UP, dist) == game_get_number(g, i - dist, j));
        }
        ASSERT(game_get_next_number(g, i, j, DOWN, dist) == game_get_number(g, (i + dist) % game_nb_rows(g), j));

        if (j < dist) {
          ASSERT(game_get_next_number(g, i, j, LEFT, dist) == game_get_number(g, i, game_nb_rows(g) + j - dist));
        } else {
          ASSERT(game_get_next_number(g, i, j, LEFT, dist) == game_get_number(g, i, j - dist));
        }
        ASSERT(game_get_next_number(g, i, j, RIGHT, dist) == game_get_number(g, i, (j + dist) % game_nb_rows(g)));
      }
    }
  }

  game_delete(g);
  return true;
}

/* ********** TEST GAME_IS_OVER ********** */
bool test_game_is_over() {
  game g = game_default();
  ASSERT(!game_is_over(g));
  game_delete(g);

  g = game_default_solution();
  ASSERT(game_is_over(g));
  game_delete(g);

  g = game_new_empty_ext(6, 6, true, true);
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if ((i % 2 == 0 && j % 2 == 0) || (i % 2 == 1 && j % 2 == 1)) {
        game_set_square(g, i, j, S_ZERO);
      } else {
        game_set_square(g, i, j, S_ONE);
      }
    }
  }
  game_print(g);
  ASSERT(game_has_error(g, 0, 0));
  game_delete(g);

  return true;
}

/* ********** TEST GAME_HAS_ERROR ********** */

bool test_game_has_error() {
  game g = game_default_solution();
  game h = game_default();
  game k = game_new_empty();
  // tests on games withouth errors
  for (int i = 0; i <= 5; i = i + 1) {
    for (int j = 0; j <= 5; j = j + 1) {
      ASSERT(game_has_error(g, i, j) == 0);
      ASSERT(game_has_error(h, i, j) == 0);
      ASSERT(game_has_error(k, i, j) == 0);
    }
  }
  game_delete(g);
  game_delete(h);

  game_set_square(k, 0, 0, S_ZERO);
  game_set_square(k, 0, 1, S_ZERO);
  game_set_square(k, 0, 2, S_ZERO);
  // test on game with errors
  ASSERT(!(game_has_error(k, 0, 0) == 0));
  ASSERT(!(game_has_error(k, 0, 1) == 0));
  ASSERT(!(game_has_error(k, 0, 2) == 0));
  game_delete(k);

  game m = game_new_empty_ext(6, 6, true, true);
  for (int i = 0; i < game_nb_cols(m); i++) {
    if (i % 2 == 0) {
      game_set_square(m, 4, i, S_ZERO);
      game_set_square(m, 5, i, S_ZERO);
    } else {
      game_set_square(m, 4, i, S_ONE);
      game_set_square(m, 5, i, S_ONE);
    }
  }
  ASSERT(!(game_has_error(m, 5, 5) == false));
  game_set_square(m, 5, 1, S_ONE);
  game_set_square(m, 4, 1, S_ONE);
  game_set_square(m, 0, 1, S_ONE);
  ASSERT(!(game_has_error(m, 5, 1) == false));
  game_delete(m);
  return true;
}

/* ********** GAME_PRINT ********** */

bool test_game_print() {
  game g = game_default();
  game_print(g);
  game_delete(g);
  return true;
}

/* ********** DUMMY ********** */

bool test_dummy() { return true; }

/* ********** USAGE ********** */

void usage(int argc, char* argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** MAIN ********** */

int main(int argc, char* argv[]) {
  if (argc == 1) usage(argc, argv);

  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool ok = false;

  // Create a struct array with test function pointers and their corresponding test names
  struct test_data {
    char* name;
    bool (*test)();
  } tests[] = {
      {"dummy", test_dummy},
      {"game_new", test_game_new},
      {"game_equal", test_game_equal},
      {"game_get_square", test_game_get_square},
      {"game_get_next_number", test_game_get_next_number},
      {"game_is_over", test_game_is_over},
      {"game_is_over", test_game_is_over},
      {"game_has_error", test_game_has_error},
      {"game_print", test_game_print},
  };

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