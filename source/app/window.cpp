#include "window.h"

window::window(app* p_app, const std::string& title, int width, int height) : _app(p_app), _width(width), _height(height) {
  // Create a window
  _window = SDL_CreateWindow(title.data(), _width, _height, 0);
  if (!_window) {
    std::cout << SDL_GetError();
  }

  // Get the window surface
  _surface = SDL_GetWindowSurface(_window);
  if (!_surface) {
    std::cout << SDL_GetError();
  }
}

window::~window() {
    SDL_DestroyWindow(_window); }

vec2<int> window::get_win_pos_pix() { 
  vec2<int> pos_pix{};

  SDL_GetWindowPosition(_window, &pos_pix.x, &pos_pix.y);
  return pos_pix; 
}

vec2<> window::get_win_pos_rel() {
  vec2<int> pos_pix{};
  vec2<> pos_rel{};

  SDL_GetWindowPosition(_window, &pos_pix.x, &pos_pix.y);

  pos_rel.x = static_cast<real>(pos_pix.x) / _app->_resolution.x;
  pos_rel.y = static_cast<real>(pos_pix.y) / _app->_resolution.y;
  return pos_rel;
}
