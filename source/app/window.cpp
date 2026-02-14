#include "window.h"
#include "helpers/sdl_hlprs.h"

using namespace smns::sdl_hlprs;

window::window(app* p_app, const std::string& title, int width, int height)
    : _app(p_app), _width(width), _height(height) {
  // Create a window
  _window = SDL_CreateWindow(title.data(), _width, _height, 0);
   check(_window);

  // Get the window surface
  _surface = SDL_GetWindowSurface(_window);
  check(_surface);

  _mode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(_window));
  check(_mode);
}

window::~window() {
    SDL_DestroyWindow(_window); }

vec2<int> window::get_win_pos_pix() { 
  vec2<int> pos_pix{};

  SDL_GetWindowPosition(_window, &pos_pix.x, &pos_pix.y);
  return pos_pix; 
}

vec2<> window::get_win_pos_rel() {
  vec2<int> pos_pix = get_win_pos_pix();
  vec2<> pos_rel{};

  pos_rel.x = static_cast<real>(pos_pix.x) / _mode->w;
  pos_rel.y = static_cast<real>(pos_pix.y) / _mode->h;
  return pos_rel;
}

real window::get_aspect_ratio() {
  real aspect_ratio = static_cast<real>(_mode->w) / _mode->h;

  return aspect_ratio;
}
