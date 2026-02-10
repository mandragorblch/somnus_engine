#pragma once
#include <SDL3/SDL.h>
#include <iostream>

namespace smns {
namespace sdl_hlprs {

void check(bool success) {
  if (!success) std::cerr << SDL_GetError();
}

}  // namespace sdl_hlprs
}  // namespace smns