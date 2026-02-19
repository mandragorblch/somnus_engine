#pragma once
#include <SDL3/SDL.h>

#include "math/defs.h"
#include "math/vec2.h"
#include "visual/color.h"
#include "app/window.h"

struct window;

struct obj {
 public:
  color<> clr{};
  vec2<> pos{};
  vec2<> vel{};
  real mass{1_r};

  obj(const color<>& color = color<>{}, vec2<> pos = vec2<>{},
      vec2<> vel = vec2<>{}, real mass = 1_r);
  obj(const obj&) = default;
  obj& operator=(const obj&) = default;
  obj(obj&&) = default;
  obj& operator=(obj&&) = default;

  // TODO math for calculating relative positions of existing windows
  virtual void draw(window* win) = 0;
};
