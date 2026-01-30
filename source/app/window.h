#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include "app/app.h"
#include "math/vec2.h"

struct window {
  struct app* _app = nullptr;
  SDL_Window* _window = nullptr;
  SDL_Surface* _surface = nullptr;
  int _width;
  int _height;

  window(app* p_app, const std::string& title, int width, int height);

  ~window();


  vec2<int> get_win_pos_pix();

  //TODO detect on wich display if diff res
  vec2<> get_win_pos_rel();
};
