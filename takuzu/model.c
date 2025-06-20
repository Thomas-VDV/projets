// SDL2 Demo by aurelien.esnard@u-bordeaux.fr
#include "model.h"

#include <SDL.h>
#include <SDL_image.h> // required to load transparent texture from PNG
#include <SDL_ttf.h>   // required to use TTF fonts
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/* **************************************************************** */

struct Env_t {
  /* PUT YOUR VARIABLES HERE */
  game g;
  SDL_Rect** rectangles;
  uint rect_size;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
  Env* env = malloc(sizeof(struct Env_t));

  game g;
  if (argc == 1) {
    g = game_default();
  } else {
    g = game_load(argv[1]);
  }

  SDL_Rect** rectangles = malloc(game_nb_rows(g) * sizeof(SDL_Rect*));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    rectangles[i] = malloc(game_nb_cols(g) * sizeof(SDL_Rect));
  }

  int rect_size = SCREEN_WIDTH / game_nb_cols(g);

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      rectangles[i][j].x = j * rect_size;
      rectangles[i][j].y = i * rect_size;
      rectangles[i][j].w = rect_size;
      rectangles[i][j].h = rect_size;
    }
  }

  env->g = g;
  env->rectangles = rectangles;

  env->rect_size = ((SCREEN_WIDTH < SCREEN_HEIGHT) ? SCREEN_WIDTH : SCREEN_HEIGHT) /
                   ((game_nb_rows(g) < game_nb_cols(g)) ? game_nb_rows(g) : game_nb_cols(g));

  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  for (uint i = 0; i < game_nb_rows(env->g); i++) {
    for (uint j = 0; j < game_nb_cols(env->g); j++) {
      square s = game_get_square(env->g, i, j);

      int color = 125; // S_EMPTY -> GREY

      if (s == S_ZERO) color = 200;           // S_ZERO -> LIGHT GREY
      if (s == S_IMMUTABLE_ZERO) color = 255; // S_IMMUTABLE_ZERO -> WHITE
      if (s == S_ONE) color = 50;             // S_ONE -> DARK GREY
      if (s == S_IMMUTABLE_ONE) color = 0;    // S_IMMUTABLE_ONE -> BLACK

      SDL_SetRenderDrawColor(ren, color, color, color, 255);
      SDL_RenderFillRect(ren, &(env->rectangles[i][j]));
    }
  }
}

/* **************************************************************** */
void getRectIndex(Env* env, SDL_Point mouse, int* i, int* j) {
  *(i) = mouse.y / env->rect_size;
  *(j) = mouse.x / env->rect_size;
}

bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  if (e->type == SDL_QUIT) {
    return true;
  }

  else if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    int i, j;
    getRectIndex(env, mouse, &i, &j);

    if (!game_is_immutable(env->g, i, j)) {
      int button = e->button.button;

      if (button == SDL_BUTTON_LEFT) {
        game_play_move(env->g, i, j, S_ONE);
      } else if (button == SDL_BUTTON_RIGHT) {
        game_play_move(env->g, i, j, S_ZERO);
      } else if (button == SDL_BUTTON_MIDDLE) {
        game_play_move(env->g, i, j, S_EMPTY);
      }

      if (game_has_error(env->g, i, j)) {
        game_undo(env->g);
      }

      /*
      switch(e->button.button){

        case SDL_BUTTON_LEFT:
          game_play_move(env->g, i, j, S_ONE);

        case SDL_BUTTON_RIGHT:
          game_play_move(env->g, i, j, S_ZERO);

        case SDL_BUTTON_MIDDLE:
          game_play_move(env->g, i, j, S_EMPTY);
      }
    */
    }
  }

  else if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_q:
        return true;

      case SDLK_ESCAPE:
        return true;

      case SDLK_r:
        game_restart(env->g);

      case SDLK_z:
        game_undo(env->g);

      case SDLK_y:
        game_redo(env->g);
    }
  }

  return false;
}

/* **************************************************************** */

void clean(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  /* PUT YOUR CODE HERE TO CLEAN MEMORY */

  if (env->rectangles) {
    for (uint i = 0; i < game_nb_rows(env->g); i++) {
      free(env->rectangles[i]);
    }
  }
  free(env->rectangles);
  game_delete(env->g);

  free(env);
}

/* **************************************************************** */