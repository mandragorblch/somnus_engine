#pragma once
#include "math/defs.h"
#include "obj/obj.h"

// the heart object
// defines as rotated by angle amd mirrored pparabola
//
struct heart : obj {
 //private:
  using obj::obj;
  using real = smns::defs::real;
  real x0{};
  real y0{};
  real phi{};
  real stretch{1.0};
  real x_scale{1.0};
  real y_scale{1.0};
  real cos_phi = std::cos(phi);
  real sin_phi = std::sin(phi);

  real top_bound{};
  real bottom_bound{};
  real left_bound{};
  real right_bound{};

 public:
  // constructor
  heart(real x0, real y0, real phi, real stretch = real{1.0}, real x_scale = real{1.0}, real y_scale = real{1.0});

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

  void set_phi(real phi);

  virtual void draw(window* win) override;
};
