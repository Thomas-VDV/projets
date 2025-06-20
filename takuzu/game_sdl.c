// SDL2 Demo by aurelien.esnard@u-bordeaux.fr
#include "game_sdl.h"

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
  SDL_Rect screen_rect;
  SDL_Rect** rectangles;
  uint rect_size;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
  game g;
  if (argc == 1) {
    g = game_default();
  } else {
    g = game_load(argv[1]);
  }

  SDL_DisplayMode dm;
  SDL_GetDesktopDisplayMode(0, &dm);
  int screen_min_size = ((dm.w < dm.h) ? dm.w : dm.h) * 0.94;

  int rect_size = screen_min_size / ((game_nb_rows(g) < game_nb_cols(g)) ? game_nb_rows(g) : game_nb_cols(g));

  int screen_width = rect_size * game_nb_cols(g);
  int screen_height = rect_size * game_nb_rows(g);

  SDL_SetWindowSize(win, screen_width, screen_height);
  SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_SetWindowTitle(win, "Takuzu");

  Env* env = malloc(sizeof(struct Env_t));

  SDL_Rect screen_rect;
  screen_rect.x = 0;
  screen_rect.y = 0;
  screen_rect.w = screen_width;
  screen_rect.h = screen_height;

  SDL_Rect** rectangles = malloc(game_nb_rows(g) * sizeof(SDL_Rect*));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    rectangles[i] = malloc(game_nb_cols(g) * sizeof(SDL_Rect));
  }

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      rectangles[i][j].x = j * rect_size;
      rectangles[i][j].y = i * rect_size;
      rectangles[i][j].w = rect_size;
      rectangles[i][j].h = rect_size;
    }
  }

  env->g = g;
  env->screen_rect = screen_rect;
  env->rectangles = rectangles;

  env->rect_size = rect_size;

  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  int min_size = (w < h) ? w : h;
  int min_row_col = (game_nb_rows(env->g) < game_nb_cols(env->g)) ? game_nb_rows(env->g) : game_nb_cols(env->g);

  env->rect_size = min_size / min_row_col;

  env->screen_rect.w = min_row_col * env->rect_size;
  env->screen_rect.h = min_row_col * env->rect_size;

  SDL_Rect immutable_rect;
  immutable_rect.w = env->rect_size / 2;
  immutable_rect.h = env->rect_size / 2;

  for (uint i = 0; i < game_nb_rows(env->g); i++) {
    for (uint j = 0; j < game_nb_cols(env->g); j++) {
      env->rectangles[i][j].x = env->rect_size * j;
      env->rectangles[i][j].y = env->rect_size * i;
      env->rectangles[i][j].w = env->rect_size;
      env->rectangles[i][j].h = env->rect_size;

      int n = game_get_number(env->g, i, j);
      int color;
      int immutable_color;

      if (n == 0) { // ZERO -> WHITE
        color = 255;
      } else if (n == 1) { // ONE -> BLACK
        color = 0;
      } else { // EMPTY -> GREY
        color = 125;
      }

      // DRAW RECTANGLE
      SDL_SetRenderDrawColor(ren, color, color, color, 255);
      SDL_RenderFillRect(ren, &(env->rectangles[i][j]));

      // DRAW IMMUTABLE INNER RECTANGLE
      if (game_is_immutable(env->g, i, j)) {
        if (n == 0) {
          immutable_color = 200;
        } else {
          immutable_color = 50;
        }
        immutable_rect.x = (env->rectangles[i][j]).x + env->rect_size / 4;
        immutable_rect.y = (env->rectangles[i][j]).y + env->rect_size / 4;

        SDL_SetRenderDrawColor(ren, immutable_color, immutable_color, immutable_color, 255);
        SDL_RenderFillRect(ren, &immutable_rect);
      }

      // ERROR DISPLAY
      if (game_has_error(env->g, i, j) && !game_is_immutable(env->g, i, j)) {
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderDrawRect(ren, &(env->rectangles[i][j]));
      }
    }
  }

  // GAME OVER DISPLAY
  if (game_is_over(env->g)) {
    SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
    SDL_RenderDrawRect(ren, &(env->screen_rect));
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
    }
  }

  else if (e->type == SDL_KEYDOWN) {
    SDL_Keycode key = e->key.keysym.sym;

    if (key == SDLK_q || key == SDLK_ESCAPE) {
      return true;
    } else if (key == SDLK_r) {
      game_restart(env->g);
    } else if (key == SDLK_z) {
      game_undo(env->g);
    } else if (key == SDLK_y) {
      game_redo(env->g);
    } else if (key == SDLK_s) {
      game_solve(env->g);
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