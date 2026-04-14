#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include "core/App.h"
#include "smns/math/vec2.h"

class Window {
 public:
  using real = smns::defs::real;
  class App* _app = nullptr;
  SDL_Window* _window = nullptr;
  SDL_Surface* _surface = nullptr;
  const SDL_DisplayMode* _mode = nullptr;

  int _width;
  int _height;
  real dy;
  real dx;

  void window_resized_callback();

  Window(App* p_app, const std::string& title, int width, int height);

  ~Window();


  vec2<int> get_win_pos_pix();

  //TODO detect on wich display if diff res
  vec2<> get_win_pos_rel();

  real get_aspect_ratio();
};
