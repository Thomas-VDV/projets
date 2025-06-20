#include "game.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

/* ********** STRUCTURE GAME ********** */
struct game_s {
  square** board;
  uint rows;
  uint cols;
  bool wrapping;
  bool unique;
  queue* undo;
  queue* redo;
};

/* ********** UTILITY FONCTION ********** */
void is_pointer_null(void* p) {
  if (p == NULL) {
    fprintf(stderr, "Null pointer\n");
    exit(EXIT_FAILURE);
  }
}

void is_index_out_of_range(cgame g, uint i, uint j) {
  if (i < 0 || i >= game_nb_rows(g) || j < 0 || j >= game_nb_cols(g)) {
    fprintf(stderr, "Index out of range\n");
    exit(EXIT_FAILURE);
  }
}

void is_dist_bigger_than_2(int dist) {
  if (dist > 2) {
    fprintf(stderr, "Dist is bigger than 2\n");
    exit(EXIT_FAILURE);
  }
}

void is_a_square(square s) {
  if (s != S_EMPTY && s != S_ZERO && s != S_ONE && s != S_IMMUTABLE_ZERO && s != S_IMMUTABLE_ONE) {
    fprintf(stderr, "Not a square\n");
    exit(EXIT_FAILURE);
  }
}

void is_an_immutable_square(square s) {
  if (s == S_IMMUTABLE_ZERO && s == S_IMMUTABLE_ONE) {
    fprintf(stderr, "The square is immutable\n");
    exit(EXIT_FAILURE);
  }
}

/* ********** GAME_NEW ********** */
game game_new(square* squares) {
  // is_pointer_null(squares);
  game g = game_new_empty();

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      g->board[i][j] = squares[i * DEFAULT_SIZE + j];
    }
  }
  return g;
}

/* ********** GAME NEW EMPTY ********** */
game game_new_empty(void) {
  game g = (game)malloc(sizeof(game) + sizeof(square**) + 2 * sizeof(uint) + 2 * sizeof(bool) + 2 * sizeof(queue*));
  // is_pointer_null((void*)g);

  g->board = (square**)malloc(DEFAULT_SIZE * sizeof(square*));
  // is_pointer_null(g->board);

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    g->board[i] = (square*)malloc(DEFAULT_SIZE * sizeof(square));
    // is_pointer_null(g->board[i]);
  }

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      g->board[i][j] = S_EMPTY;
    }
  }

  g->rows = DEFAULT_SIZE;
  g->cols = DEFAULT_SIZE;
  g->wrapping = false;
  g->unique = false;

  g->undo = queue_new();
  g->redo = queue_new();

  return g;
}

/* ********** GAME_COPY ********** */
game game_copy(cgame g) {
  // is_pointer_null((void*)g);

  game g_copy = game_new_empty_ext(game_nb_rows(g), game_nb_cols(g), game_is_wrapping(g), game_is_unique(g));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      game_set_square(g_copy, i, j, game_get_square(g, i, j));
    }
  }
  return g_copy;
}

/* ********** GAME_EQUAL ********** */
bool game_equal(cgame g1, cgame g2) {
  // is_pointer_null((void*)g1);
  // is_pointer_null((void*)g2);

  if (game_nb_rows(g1) != game_nb_rows(g2) || game_nb_cols(g1) != game_nb_cols(g2) ||
      game_is_wrapping(g1) != game_is_wrapping(g2) || game_is_unique(g1) != game_is_unique(g2)) {
    return false;
  }

  for (uint i = 0; i < game_nb_rows(g1); i++) {
    for (uint j = 0; j < game_nb_cols(g1); j++) {
      if (g1->board[i][j] != g2->board[i][j]) {
        return false;
      }
    }
  }
  return true;
}

/* ********** GAME_DELETE ********** */
void game_delete(game g) {
  // is_pointer_null(g);

  if (g->board != NULL) {
    for (uint i = 0; i < game_nb_rows(g); i++) {
      free(g->board[i]);
    }
    free(g->board);
  }

  while (!queue_is_empty(g->undo)) {
    game_delete(queue_pop_head(g->undo));
  }
  while (!queue_is_empty(g->redo)) {
    game_delete(queue_pop_head(g->redo));
  }
  free(g->undo);
  free(g->redo);
  free(g);
}

/* ********** GAME_SET_SQUARE ********** */
void game_set_square(game g, uint i, uint j, square s) {
  // is_pointer_null((void*)g);
  // is_index_out_of_range(g, i, j);
  // is_a_square(s);
  // is_an_immutable_square(s);

  g->board[i][j] = s;
}

/* ********** GAME_GET_SQUARE ********** */
square game_get_square(cgame g, uint i, uint j) {
  // is_pointer_null((void*)g);
  // is_index_out_of_range(g, i, j);
  return g->board[i][j];
}

/* ********** GAME_GET_NUMBER ********** */
int game_get_number(cgame g, uint i, uint j) {
  // is_pointer_null((void*)g);
  // is_index_out_of_range(g, i, j);

  square s = game_get_square(g, i, j);
  if (s == S_ZERO || s == S_IMMUTABLE_ZERO) {
    return 0;
  } else if (s == S_ONE || s == S_IMMUTABLE_ONE) {
    return 1;
  } else {
    return -1;
  }
}

/* ********** GAME_GET_NEXT_SQUARE ********** */
int game_get_next_square(cgame g, uint i, uint j, direction dir, uint dist) {
  // is_pointer_null((void*)g);
  // is_index_out_of_range(g, i, j);
  // is_dist_bigger_than_2(dist);

  if (dir == UP) {
    if (i >= dist) {
      return game_get_square(g, i - dist, j);
    } else if (game_is_wrapping(g)) {
      return game_get_square(g, game_nb_rows(g) + i - dist, j);
    }

  } else if (dir == DOWN) {
    if (i + dist < game_nb_rows(g)) {
      return game_get_square(g, i + dist, j);
    } else if (i + dist >= game_nb_rows(g) && game_is_wrapping(g)) {
      return game_get_square(g, (i + dist) % game_nb_rows(g), j);
    }

  } else if (dir == LEFT) {
    if (j >= dist) {
      return game_get_square(g, i, j - dist);
    } else if (j < dist && game_is_wrapping(g)) {
      return game_get_square(g, i, game_nb_cols(g) + j - dist);
    }

  } else if (dir == RIGHT) {
    if (j + dist < game_nb_cols(g)) {
      return game_get_square(g, i, j + dist);
    } else if (game_is_wrapping(g)) {
      return game_get_square(g, i, (j + dist) % game_nb_cols(g));
    }
  }
  return -1;
}

/* ********** GAME_GET_NEXT_NUMBER ********** */
int game_get_next_number(cgame g, uint i, uint j, direction dir, uint dist) {
  // is_pointer_null((void*)g);
  // is_index_out_of_range(g, i, j);
  // is_dist_bigger_than_2(dist);

  square s = game_get_next_square(g, i, j, dir, dist);
  if (s == S_ZERO || s == S_IMMUTABLE_ZERO) {
    return 0;
  } else if (s == S_ONE || s == S_IMMUTABLE_ONE) {
    return 1;
  } else {
    return -1;
  }
}

/* ********** GAME_IS_EMPTY ********** */
bool game_is_empty(cgame g, uint i, uint j) { return (g->board[i][j] == S_EMPTY); }

/* ********** GAME_IS_IMMUTABLE ********** */
bool game_is_immutable(cgame g, uint i, uint j) {
  // is_pointer_null((void*)g);
  // is_index_out_of_range(g, i, j);
  return (game_get_square(g, i, j) == S_IMMUTABLE_ONE || game_get_square(g, i, j) == S_IMMUTABLE_ZERO);
}

/* ********** GAME_HAS_ERROR ********** */
int _count_in_row(cgame g, int nb, uint i, int nb_cols) {
  // assert(g);
  // assert(i < game_nb_rows(g));
  // assert(nb == 0 || nb == 1);
  uint count = 0;
  for (uint jj = 0; jj < nb_cols; jj++) {
    int val = game_get_number(g, i, jj);
    if (val == nb) count++;
  }
  return count;
}

/* ************************************************************************** */

int _count_in_col(cgame g, int nb, uint j, int nb_rows) {
  // assert(g);
  // assert(j < game_nb_cols(g));
  // assert(nb == 0 || nb == 1);
  uint count = 0;
  for (uint ii = 0; ii < nb_rows; ii++) {
    int val = game_get_number(g, ii, j);
    if (val == nb) count++;
  }
  return count;
}

/* ************************************************************************** */

bool _three_consecutive_row(cgame g, uint i, uint j) {
  // assert(g);
  // assert(i < game_nb_rows(g));
  // assert(j < game_nb_cols(g));

  int h0 = game_get_number(g, i, j);
  if (h0 == -1) return false;

  int l1 = game_get_next_number(g, i, j, LEFT, 1);
  int l2 = game_get_next_number(g, i, j, LEFT, 2);
  int r1 = game_get_next_number(g, i, j, RIGHT, 1);
  int r2 = game_get_next_number(g, i, j, RIGHT, 2);

  if (l2 == h0 && l1 == h0) return true;
  if (l1 == h0 && r1 == h0) return true;
  if (r1 == h0 && r2 == h0) return true;

  return false;
}

/* ************************************************************************** */

bool _three_consecutive_col(cgame g, uint i, uint j) {
  // assert(g);
  // assert(i < game_nb_cols(g));
  // assert(j < game_nb_rows(g));
  int h0 = game_get_number(g, i, j);
  if (h0 == -1) return false;

  int u1 = game_get_next_number(g, i, j, UP, 1);
  int u2 = game_get_next_number(g, i, j, UP, 2);
  int d1 = game_get_next_number(g, i, j, DOWN, 1);
  int d2 = game_get_next_number(g, i, j, DOWN, 2);

  if (u2 == h0 && u1 == h0) return true;
  if (u1 == h0 && d1 == h0) return true;
  if (d1 == h0 && d2 == h0) return true;

  return false;
}

/* ************************************************************************** */

/* compare rows i0 and i1 and return true if they are equal. */
static bool _cmp_row(cgame g, uint i0, uint i1, int nb_cols) {
  // assert(g);
  // assert(i0 < game_nb_rows(g));
  // assert(i1 < game_nb_rows(g));

  for (uint jj = 0; jj < nb_cols; jj++) {
    int val0 = game_get_number(g, i0, jj);
    if (val0 == -1) return false;
    int val1 = game_get_number(g, i1, jj);
    if (val1 == -1) return false;
    if (val0 != val1) return false;
  }

  return true;
}

/* ************************************************************************** */

/* compare cols j0 and j1 and return true if they are equal. */
static bool _cmp_col(cgame g, uint j0, uint j1, int nb_rows) {
  // assert(g);
  // assert(j0 < game_nb_cols(g));
  // assert(j1 < game_nb_cols(g));

  for (uint ii = 0; ii < nb_rows; ii++) {
    int val0 = game_get_number(g, ii, j0);
    if (val0 == -1) return false;
    int val1 = game_get_number(g, ii, j1);
    if (val1 == -1) return false;
    if (val0 != val1) return false;
  }

  return true;
}

/* ************************************************************************** */

bool _unique_row(cgame g, uint i, bool forward, int nb_cols, int nb_rows) {
  // assert(g);
  // assert(i < game_nb_rows(g));

  /* if forward is true, start camparison with row ii > i */
  uint start = forward ? (i + 1) : 0;

  /* compare row i with row ii */
  for (uint ii = start; ii < nb_rows; ii++) {
    if (i == ii) continue; // skip it
    bool equal = _cmp_row(g, i, ii, nb_cols);
    if (equal) return false;
  }

  return true;
}

/* ************************************************************************** */

bool _unique_col(cgame g, uint j, bool forward, int nb_cols, int nb_rows) {
  // assert(g);
  // assert(j < game_nb_cols(g));

  /* if forward is true, start camparison with col jj > j */
  uint start = forward ? (j + 1) : 0;

  /* compare row j with row jj */
  for (uint jj = start; jj < nb_cols; jj++) {
    if (j == jj) continue; // skip it
    bool equal = _cmp_col(g, j, jj, nb_rows);
    if (equal) return false;
  }

  return true;
}

int game_has_error(cgame g, uint i, uint j) {
  // assert(g);
  // assert(i < game_nb_rows(g));
  // assert(j < game_nb_cols(g));
  int nb_rows = game_nb_rows(g);
  int nb_cols = game_nb_cols(g);

  int num = game_get_number(g, i, j);

  // no error occurs if the square is empty
  if (num == -1) return 0;

  // check number of ones and zeros in row i
  uint nb_zeros = _count_in_row(g, 0, i, nb_cols);
  uint nb_ones = _count_in_row(g, 1, i, nb_cols);
  if ((num == 0) && (nb_zeros > (nb_cols / 2))) return 1;
  if ((num == 1) && (nb_ones > (nb_cols / 2))) return 1;

  // check number of ones and zeros in col j
  nb_zeros = _count_in_col(g, 0, j, nb_rows);
  nb_ones = _count_in_col(g, 1, j, nb_rows);
  if ((num == 0) && (nb_zeros > (nb_cols / 2))) return 2;
  if ((num == 1) && (nb_ones > (nb_cols / 2))) return 2;

  // check three consecutive in row i
  bool cons_row = _three_consecutive_row(g, i, j);
  if (cons_row) return 3;

  // check three consecutive in col j
  bool cons_col = _three_consecutive_col(g, i, j);
  if (cons_col) return 4;

  if (g->unique) {
    // check row i is unique
    bool uniq_row = _unique_row(g, i, false, nb_cols, nb_rows);
    if (!uniq_row) return 5;

    // check col j is unique
    bool uniq_col = _unique_col(g, j, false, nb_cols, nb_rows);
    if (!uniq_col) return 6;
  }

  return 0;
}

/* ********** GAME_CHECK_MOVE********** */
bool game_check_move(cgame g, uint i, uint j, square s) {
  // is_pointer_null((void*)g);
  if (i < 0 || i >= game_nb_rows(g) || j < 0 || j >= game_nb_cols(g) || (s != S_EMPTY && s != S_ONE && s != S_ZERO) ||
      game_get_square(g, i, j) == S_IMMUTABLE_ONE || game_get_square(g, i, j) == S_IMMUTABLE_ZERO) {
    return false;
  }
  return true;
}

/* ********** GAME_PLAY_MOVE ********** */
void game_play_move(game g, uint i, uint j, square s) {
  is_pointer_null((void*)g);
  is_index_out_of_range(g, i, j);
  is_a_square(s);
  is_an_immutable_square(s);

  while (!queue_is_empty(g->redo)) {
    game_delete(queue_pop_head(g->redo));
  }
  queue_push_head(g->undo, game_copy(g));
  g->board[i][j] = s;
}

/* ********** GAME_IS_OVER********** */
bool game_is_over(cgame g) {
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_rows(g); j++) {
      if (game_get_square(g, i, j) == S_EMPTY || game_has_error(g, i, j)) {
        return false;
      }
    }
  }

  return true;
}

/* ********** GAME_RESTART ********** */
void game_restart(game g) {
  // is_pointer_null((void*)g);
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) != S_IMMUTABLE_ZERO && game_get_square(g, i, j) != S_IMMUTABLE_ONE) {
        game_set_square(g, i, j, S_EMPTY);
      }
    }
  }

  while (!queue_is_empty(g->undo)) {
    game_delete(queue_pop_head(g->undo));
  }

  while (!queue_is_empty(g->redo)) {
    game_delete(queue_pop_head(g->redo));
  }
}

/* ********** GAME_NEW_EXT ********** */
game game_new_ext(uint nb_rows, uint nb_cols, square* squares, bool wrapping, bool unique) {
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, unique);
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      g->board[i][j] = squares[i * nb_cols + j];
    }
  }
  return g;
}

/* ********** GAME_NEW_EMPTY_EXT ********** */
game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping, bool unique) {
  game g = (game)malloc(sizeof(game) + sizeof(square**) + 2 * sizeof(uint) + 2 * sizeof(bool) + 2 * sizeof(queue*));
  is_pointer_null((void*)g);
  g->board = (square**)malloc(nb_rows * sizeof(square*));
  for (uint i = 0; i < nb_rows; i++) {
    g->board[i] = (square*)malloc(nb_cols * sizeof(square));
    is_pointer_null(g->board[i]);
  }

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      g->board[i][j] = S_EMPTY;
    }
  }

  g->rows = nb_rows;
  g->cols = nb_cols;
  g->wrapping = wrapping;
  g->unique = unique;

  g->undo = queue_new();
  g->redo = queue_new();

  return g;
}

/* ********** GAME_NB_ROWS ********** */
uint game_nb_rows(cgame g) {
  // is_pointer_null((void*)g);
  return g->rows;
}

/* ********** GAME_NB_COLS ********** */
uint game_nb_cols(cgame g) {
  // is_pointer_null((void*)g);
  return g->cols;
}

/* ********** GAME_IS_WRAPPING ********** */
bool game_is_wrapping(cgame g) {
  is_pointer_null((void*)g);
  return g->wrapping;
}

/* ********** GAME_IS_UNIQUE ********** */
bool game_is_unique(cgame g) {
  // is_pointer_null((void*)g);
  return g->unique;
}

/* ********** GAME_UNDO ********** */
void game_undo(game g) {
  if (queue_is_empty(g->undo)) {
    return;
  }
  queue_push_head(g->redo, game_copy(g));
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      game_set_square(g, i, j, game_get_square(queue_peek_head(g->undo), i, j));
    }
  }
  game_delete(queue_pop_head(g->undo));
}

/* ********** GAME_REDO ********** */
void game_redo(game g) {
  if (queue_is_empty(g->redo)) {
    return;
  }
  queue_push_head(g->undo, game_copy(g));
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      game_set_square(g, i, j, game_get_square(queue_peek_head(g->redo), i, j));
    }
  }
  game_delete(queue_pop_head(g->redo));
}
