#pragma once
#include <SDL3/SDL.h>

#include "math/defs.h"
#include "math/vec2.h"
#include "visual/color.h"

struct obj {
 public:
  vec2<> pos{};
  vec2<> vel{};
  real mass{1_r};
  color clr{};

  obj() = default;
  obj(const obj&) = default;
  obj& operator=(const obj&) = default;
  obj(obj&&) = default;
  obj& operator=(obj&&) = default;

  // TODO math for calculating relative positions of existing windows
  virtual void draw(SDL_Surface* surf) = 0;
};
