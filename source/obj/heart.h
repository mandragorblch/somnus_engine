#pragma once
#include "math/defs.h"
#include "obj/base_objs/renderer.h"
#include <functional>

enum class HEART_TYPES : uint8_t {
  PARABOLA,
  CIRCLE_ATANH,
  CIRCLE_SQRT,
  CIRCLE_LINE,
  CIRCLE_ASIN,
  CIRCLE_PARABOLA
};

// the heart object
// defines as rotated by angle and mirrored parabola
template<HEART_TYPES type>
class heart : public renderer<RENDER_TYPE::ANALYTICAL, heart<type>> {};

template<>
class heart<HEART_TYPES::PARABOLA> : public renderer<RENDER_TYPE::ANALYTICAL, heart<HEART_TYPES::PARABOLA>> {
 public:
  using real = smns::defs::real;
  using render_t = renderer<RENDER_TYPE::ANALYTICAL, heart<HEART_TYPES::PARABOLA>>;
  using render_t::render_t;
  real x0{};
  real y0{};
  real phi{};
  real stretch{1.0};
  real phase{};
  real cos_phi = std::cos(phi);
  real sin_phi = std::sin(phi);
  

  real func(real x, real y);
 
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

  virtual void calc_bounds() override;

  void set_phi(real phi);

  virtual void draw(window* win) override;
};



//template<>
//class heart<HEART_TYPES::CIRCLE_ATANH> : public obj {
// public:
//  using obj::obj;
//  using real = smns::defs::real;
//  real x0{};
//  real y0{};
//  real r{};
//  real param{1.0};
//
//
//  // constructor
//  heart(real x0, real y0, real r, real param = real{1.0}, real x_scale = real{1.0}, real y_scale = real{1.0});
//
//  // copy constructor
//  heart(const heart&) = default;
//
//  // copy assignment
//  heart& operator=(const heart&) = default;
//
//  // move constructor
//  heart(heart&&) noexcept = default;
//
//  // move assignment
//  heart& operator=(heart&&) noexcept = default;
//
//  // destructor
//  ~heart() = default;
//
//  void calc_bounds();
//
//  void set_phi(real phi);
//
//  void draw(window* win);
//};
