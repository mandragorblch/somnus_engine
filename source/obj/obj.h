#pragma once
#include <SDL3/SDL.h>

#include "math/defs.h"
#include "math/vec2.h"
#include "visual/color.h"

struct window;

struct obj {
 public:
  color<> clr{};
  vec2<> pos{};
  vec2<> vel{};
  real mass{1.0};

  real top_bound{};
  real bottom_bound{};
  real left_bound{};
  real right_bound{};

  obj(const color<>& color = color<>{}, vec2<> pos = vec2<>{},
      vec2<> vel = vec2<>{}, real mass = real{1.0});
  obj(const obj&) = default;
  obj& operator=(const obj&) = default;
  obj(obj&&) = default;
  obj& operator=(obj&&) = default;

  // TODO math for calculating relative positions of existing windows
  virtual void draw(window* win) = 0;
  virtual void calc_bounds() = 0;
};
