#include "window.h"

#include "helpers/sdl_hlprs.h"

using namespace smns::sdl_hlprs;

void Window::window_resized_callback() {
  SDL_GetWindowSizeInPixels(_window, &_width, &_height);
  dy = map_to_window_relative_height(1, _height);
  dx = map_to_window_relative_width(1, _width);
}

Window::Window(App* p_app, const std::string& title, int width, int height)
    : _app(p_app) {
  // Create a Window
  _window = SDL_CreateWindow(title.data(), width, height, 0);
  check(_window);

  // Get the Window surface
  _surface = SDL_GetWindowSurface(_window);
  check(_surface);

  _mode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(_window));
  check(_mode);
	//TODO detup callback
  window_resized_callback();
}

Window::~Window() { SDL_DestroyWindow(_window); }

vec2<int> Window::get_win_pos_pix() {
  vec2<int> pos_pix{};

  SDL_GetWindowPosition(_window, &pos_pix.x, &pos_pix.y);
  return pos_pix;
}

vec2<> Window::get_win_pos_rel() {
  vec2<int> pos_pix = get_win_pos_pix();
  vec2<> pos_rel{};

  pos_rel.x = static_cast<real>(pos_pix.x) / _mode->w;
  pos_rel.y = static_cast<real>(pos_pix.y) / _mode->h;
  return pos_rel;
}

::smns::defs::real Window::get_aspect_ratio() {
  real aspect_ratio = static_cast<real>(_mode->w) / _mode->h;

  return aspect_ratio;
}
