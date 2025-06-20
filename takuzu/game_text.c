#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

void help() {
  printf(
      "action : help\n- press 'w <i> <j>' to put a zero/white at square "
      "(i,j)\n");
  printf("- press 'b <i> <j>' to put a one/black at square (i,j)\n");
  printf("- press 'e <i> <j>' to empty square (i,j)\n");
  printf("- press 'z' to undo the last move\n- press 'q' to quit\n");
  printf("- press 'y' to redo the last move\n- press 'q' to quit\n");
  printf("- press 'r' to restart\n- press 'q' to quit\n");
  printf("- press 's' to save the game\n- press 'q' to quit\n");
}

int main(int argc, char* argv[]) {
  printf("\n");
  game g;
  if (argc == 2) {
    g = game_load(argv[1]);
  } else if (argc == 1) {
    g = game_default();
  } else {
    printf("Too many arguments\n");
    return EXIT_FAILURE;
  }
  while (!game_is_over(g)) {
    game_print(g);
    for (int i = 0; i < game_nb_rows(g); i++) {
      for (int j = 0; j < game_nb_cols(g); j++) {
        if (game_has_error(g, i, j)) {
          printf("Error at square (%d,%d)\n", i, j);
        }
      }
    }
    char key;
    printf("? [h for help]\n");
    int test = scanf(" %c", &key);
    if (test == 0) {
      printf("Invalid input\n");
      return EXIT_FAILURE;
    }

    if (key == 'h') {
      help();
    } else if (key == 'r') {
      printf("action: restart\n");
      game_restart(g);
    } else if (key == 'q') {
      printf("shame\n");
      game_delete(g);
      return EXIT_SUCCESS;
    } else if (key == 'z') {
      game_undo(g);
    } else if (key == 's') {
      char* file_name = malloc(sizeof(char*));
      printf("Name of the file to save:");
      test = scanf("%s", file_name);
      if (test == 0) {
        printf("Invalid input\n");
        return EXIT_FAILURE;
      }
      game_save(g, file_name);
      free(file_name);
      return EXIT_SUCCESS;
    } else if (key == 'y') {
      game_redo(g);
    } else if (key == 'w' || key == 'b' || key == 'e') {
      int i, j;
      int test = scanf("%d %d", &i, &j);
      if (test == 0) {
        printf("Invalid input\n");
        return EXIT_FAILURE;
      }
      if (game_check_move(g, i, j, S_ZERO)) {
        if (key == 'w') {
          game_play_move(g, i, j, S_ZERO);
          printf("action: play move '%c' into square (%d,%d)\n", key, i, j);
        } else if (key == 'b') {
          game_play_move(g, i, j, S_ONE);
          printf("action: play move '%c' into square (%d,%d)\n", key, i, j);
        } else if (key == 'e') {
          game_play_move(g, i, j, S_EMPTY);
          printf("action: play move '%c' into square (%d,%d)\n", key, i, j);
        }
      }
    }
  }
  game_print(g);
  printf("congratulation\n");
  game_delete(g);

  return EXIT_SUCCESS;
}