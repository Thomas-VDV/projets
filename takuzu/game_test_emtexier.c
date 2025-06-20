#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/* ********** ASSERT ********** */

#define ASSERT(expr)                                                                  \
  do {                                                                                \
    if ((expr) == 0) {                                                                \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, #expr); \
      abort();                                                                        \
    }                                                                                 \
  } while (0)

/* ********** TEST DUMMY ********** */

bool test_dummy() { return true; }

/* ********** TEST GAME SET SQUARE  ********** */

bool test_game_set_square() {
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      game g = game_new_empty();
      game_set_square(g, i, j, S_ONE);
      ASSERT(S_ONE == game_get_square(g, i, j));
      game_set_square(g, i, j, S_IMMUTABLE_ONE);
      ASSERT(S_IMMUTABLE_ONE == game_get_square(g, i, j));
      game_set_square(g, i, j, S_IMMUTABLE_ZERO);
      ASSERT(S_IMMUTABLE_ZERO == game_get_square(g, i, j));
      game_set_square(g, i, j, S_ZERO);
      ASSERT(S_ZERO == game_get_square(g, i, j));
      game_delete(g);
    }
  }
  square s[DEFAULT_SIZE][DEFAULT_SIZE];
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      s[i][j] = S_ONE;
    }
  }
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      game g = game_new(*s);
      game_set_square(g, i, j, S_EMPTY);
      ASSERT(S_EMPTY == game_get_square(g, i, j));
      game_delete(g);
    }
  }
  return true;
}

/* ********** TEST GAME COPY  ********** */

bool test_game_copy() {
  game g = game_new_empty_ext(6, 6, true, true);
  ASSERT(g);
  game g_copy = game_copy(g);
  ASSERT(g_copy);
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      ASSERT(game_get_square(g, i, j) == game_get_square(g_copy, i, j));
    }
  }
  ASSERT(game_is_unique(g) == game_is_unique(g_copy));
  ASSERT(game_is_wrapping(g) == game_is_wrapping(g_copy));

  ASSERT(game_equal(g, g_copy));

  ASSERT(game_nb_cols(g) == game_nb_cols(g_copy));
  ASSERT(game_nb_rows(g) == game_nb_rows(g_copy));

  game_delete(g);
  game_delete(g_copy);
  return true;
}

/* ********** TEST GAME GET NEXT SQUARE ********** */

bool test_game_get_next_square() {
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
          ASSERT(game_get_next_square(g, i, j, UP, dist) == game_get_square(g, game_nb_rows(g) + i - dist, j));
        } else {
          ASSERT(game_get_next_square(g, i, j, UP, dist) == game_get_square(g, i - dist, j));
        }
        ASSERT(game_get_next_square(g, i, j, DOWN, dist) == game_get_square(g, (i + dist) % game_nb_rows(g), j));

        if (j < dist) {
          ASSERT(game_get_next_square(g, i, j, LEFT, dist) == game_get_square(g, i, game_nb_rows(g) + j - dist));
        } else {
          ASSERT(game_get_next_square(g, i, j, LEFT, dist) == game_get_square(g, i, j - dist));
        }
        ASSERT(game_get_next_square(g, i, j, RIGHT, dist) == game_get_square(g, i, (j + dist) % game_nb_rows(g)));
      }
    }
  }

  game_delete(g);
  return true;
}

/* ********** TEST GAME IS IMMUTABLE ********** */

bool test_game_is_immutable() {
  game g = game_default();
  ASSERT(g);
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_square(g, i, j) == S_IMMUTABLE_ONE || game_get_square(g, i, j) == S_IMMUTABLE_ZERO)
        ASSERT(game_is_immutable(g, i, j) == true);
      else
        ASSERT(game_is_immutable(g, i, j) == false);
    }
  }
  game_delete(g);
  return true;
}

/* ********** TEST GAME PLAY MOVE ********** */

bool test_game_play_move() {
  game g = game_default();
  ASSERT(g);

  game_play_move(g, 0, 0, S_ONE);
  ASSERT(game_get_square(g, 0, 0) == S_ONE);

  game_delete(g);
  return true;
}

/* ********** TEST GAME DEFAULT SOLUTION ********** */

bool test_game_default_solution() {
  game g_solution = game_default_solution();
  ASSERT(g_solution);

  bool test = game_is_over(g_solution);
  game_delete(g_solution);
  return test;
}

/* ********** TEST GAME IS UNIQUE ********** */

bool test_game_is_unique() {
  game g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true, true);

  // test unique = true
  ASSERT(g);
  ASSERT(game_is_unique(g));
  game_delete(g);

  // test unique = false
  g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true, false);
  ASSERT(!game_is_unique(g));

  game_delete(g);
  return true;
}

/* ********** TEST GAME IS WRAPPING ********** */

bool test_game_is_wrapping() {
  game g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true, true);

  // test wrapping = true
  ASSERT(g);
  ASSERT(game_is_wrapping(g));
  game_delete(g);

  // test wrapping = false
  g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, true);
  ASSERT(!game_is_wrapping(g));

  game_delete(g);
  return true;
}

/* ********** TEST GAME LOAD ********** */
bool test_game_load() {
  game g_default = game_default();
  ASSERT(g_default);

  game g_load_default = game_load("default.txt");
  ASSERT(g_load_default);
  ASSERT(game_equal(g_default, g_load_default));

  game_delete(g_default);
  game_delete(g_load_default);

  game g_solution = game_default_solution();
  ASSERT(g_solution);

  game g_load_solution = game_load("solution.txt");
  ASSERT(g_load_solution);
  ASSERT(game_equal(g_solution, g_load_solution));

  game_delete(g_solution);
  game_delete(g_load_solution);

  return true;
}

/* ********** TEST GAME SAVE ********** */
bool test_game_save() {
  game g_default = game_default();
  ASSERT(g_default);

  game_save(g_default, "empty_file.txt");
  game g_default_load = game_load("empty_file.txt");
  ASSERT(game_equal(g_default, g_default_load));

  game_delete(g_default);
  game_delete(g_default_load);

  game g_solution = game_default_solution();
  ASSERT(g_solution);
  char* file_name = "save.txt";
  game_save(g_solution, file_name);
  game g_solution_load = game_load(file_name);
  ASSERT(game_equal(g_solution, g_solution_load));

  game_delete(g_solution);
  game_delete(g_solution_load);

  return true;
}
/* ********** TEST_GAME_SOLVE ********** */
bool test_game_solve() {
  game g = game_new_empty_ext(4, 4, false, true);
  ASSERT(g);
  game_solve(g);
  ASSERT(game_is_over(g));
  game_delete(g);

  g = game_default();
  ASSERT(g);
  game_solve(g);
  ASSERT(game_is_over(g));
  game_delete(g);

  g = game_load("4x4_0.txt");
  ASSERT(g);
  ASSERT(!game_solve(g));
  game_delete(g);

  g = game_load("4x4_1.txt");
  ASSERT(g);
  game_solve(g);
  ASSERT(game_is_over(g));
  game_delete(g);

  g = game_load("4x4_2.txt");
  ASSERT(g);
  game_solve(g);
  ASSERT(game_is_over(g));
  game_delete(g);

  g = game_load("4x4_72.txt");
  ASSERT(g);
  game_solve(g);
  ASSERT(game_is_over(g));
  game_delete(g);

  g = game_load("4x4_90.txt");
  ASSERT(g);
  game_solve(g);
  ASSERT(game_is_over(g));
  game_delete(g);

  return true;
}

/* ********** TEST_GAME_NB_SOLUTIONS ********** */
bool test_game_nb_solutions() {
  game g = game_load("4x4_0.txt");
  ASSERT(g);
  int test = game_nb_solutions(g);
  ASSERT(test == 0);
  game_delete(g);

  g = game_load("4x4_1.txt");
  ASSERT(g);
  test = game_nb_solutions(g);
  ASSERT(test == 1);
  game_delete(g);

  g = game_load("4x4_2.txt");
  ASSERT(g);
  test = game_nb_solutions(g);
  ASSERT(test == 2);
  game_delete(g);

  g = game_load("4x4_72.txt");
  ASSERT(g);
  test = game_nb_solutions(g);
  ASSERT(test == 72);
  game_delete(g);

  g = game_load("4x4_90.txt");
  ASSERT(g);
  test = game_nb_solutions(g);
  ASSERT(test == 90);
  game_delete(g);

  g = game_new_empty_ext(6, 6, false, false);
  ASSERT(g);
  test = game_nb_solutions(g);
  ASSERT(test == 11222);
  game_delete(g);

  /*game g = game_new_empty_ext(8, 8, false, false);
  ASSERT(g);
  int test = game_nb_solutions(g);
  printf("%d\n", test);
  ASSERT(test == 12413918);
  game_delete(g);*/

  return true;
}

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
  } tests[] = {{"dummy", test_dummy},
               {"game_set_square", test_game_set_square},
               {"game_copy", test_game_copy},
               {"game_get_next_square", test_game_get_next_square},
               {"game_is_immutable", test_game_is_immutable},
               {"game_play_move", test_game_play_move},
               {"game_default_solution", test_game_default_solution},
               {"game_is_unique", test_game_is_unique},
               {"game_is_wrapping", test_game_is_wrapping},
               {"game_load", test_game_load},
               {"game_save", test_game_save},
               {"game_solve", test_game_solve},
               {"game_nb_solutions", test_game_nb_solutions}};
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
