#pragma once
#include "math/defs.h"
#include "obj/obj.h"

using namespace smns::defs;
// the heart object
// defines as rotated by angle amd mirrored pparabola
//
struct heart : obj {
 private:
  real x0{};
  real y0{};
  real phi{};
  real x_scale{1_r};
  real y_scale{1_r};

  real top_bound{};
  real bottom_bound{};
  real left_bound{};
  real right_bound{};

 public:
  // constructor
  heart(real x0, real y0, real phi, real x_scale, real y_scale);

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

  void calc_bounds();

  virtual void draw(SDL_Surface* surf) override;
};
