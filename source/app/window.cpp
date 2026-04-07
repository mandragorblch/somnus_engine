#include "window.h"

#include "helpers/sdl_hlprs.h"

using namespace smns::sdl_hlprs;

void window_t::window_resized_callback() {
  SDL_GetWindowSizeInPixels(_window, &_width, &_height);
  dy = map_to_screen_relative_height(1, this) -
       map_to_screen_relative_height(0, this);
  dx = map_to_screen_relative_width(1, this) -
       map_to_screen_relative_width(0, this);
}

window_t::window_t(app* p_app, const std::string& title, int width, int height)
    : _app(p_app) {
  // Create a window_t
  _window = SDL_CreateWindow(title.data(), width, height, 0);
  check(_window);

  // Get the window_t surface
  _surface = SDL_GetWindowSurface(_window);
  check(_surface);

  _mode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(_window));
  check(_mode);

  window_resized_callback();
}

window_t::~window_t() { SDL_DestroyWindow(_window); }

vec2<int> window_t::get_win_pos_pix() {
  vec2<int> pos_pix{};

  SDL_GetWindowPosition(_window, &pos_pix.x, &pos_pix.y);
  return pos_pix;
}

vec2<> window_t::get_win_pos_rel() {
  vec2<int> pos_pix = get_win_pos_pix();
  vec2<> pos_rel{};

  pos_rel.x = static_cast<real>(pos_pix.x) / _mode->w;
  pos_rel.y = static_cast<real>(pos_pix.y) / _mode->h;
  return pos_rel;
}

::smns::defs::real window_t::get_aspect_ratio() {
  real aspect_ratio = static_cast<real>(_mode->w) / _mode->h;

  return aspect_ratio;
}
