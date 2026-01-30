#pragma once
#include "math/defs.h"
#include "obj/obj.h"

using namespace smns::defs;
// the heart object
// defines as rotated by angle amd mirrored pparabola
//
struct heart : obj {
  real x_scale{1_r};
  real x0{};
  real y_scale{1_r};
  real y0{};
  real phi{};

  // constructor
  heart() = default;

  // copy constructor
  heart(const heart&) = default;

  // copy assignment
  heart& operator=(const heart&) = default;

  // move constructor
  heart(heart&&) noexcept = default;

  // move assignment
  heart& operator=(heart&&) noexcept = default;

  // destructor
  ~heart() = default;

  virtual void draw(SDL_Surface* surf) override;
};
