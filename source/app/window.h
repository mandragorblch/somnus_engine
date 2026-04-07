#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include "app/app.h"
#include "smns/math/vec2.h"

class window_t {
 public:
  using real = smns::defs::real;
  class app* _app = nullptr;
  SDL_Window* _window = nullptr;
  SDL_Surface* _surface = nullptr;
  const SDL_DisplayMode* _mode = nullptr;

  int _width;
  int _height;
  real dy;
  real dx;

  void window_resized_callback();

  window_t(app* p_app, const std::string& title, int width, int height);

  ~window_t();


  vec2<int> get_win_pos_pix();

  //TODO detect on wich display if diff res
  vec2<> get_win_pos_rel();

  real get_aspect_ratio();
};
