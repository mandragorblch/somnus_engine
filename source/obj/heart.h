#pragma once
#include "math/defs.h"
#include "obj/obj.h"

enum class HEART_TYPES : uint8_t {
  PARABOLA,
  CIRCLE_ATANH,
  CIRCLE_SQRT,
  CIRCLE_LINE,
  CIRCLE_ASIN
};

// the heart object
// defines as rotated by angle and mirrored parabola
template<HEART_TYPES type>
struct heart : obj {};

template<>
struct heart<HEART_TYPES::PARABOLA> : obj {
 //private:
  using obj::obj;
  using real = smns::defs::real;
  real x0{};
  real y0{};
  real phi{};
  real stretch{1.0};
  real x_scale{1.0};
  real y_scale{1.0};
  real phase = 0_r;
  real cos_phi = std::cos(phi);
  real sin_phi = std::sin(phi);

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

  virtual void calc_bounds() override;

  void set_phi(real phi);

  virtual void draw(window* win) override;
};



template<>
struct heart<HEART_TYPES::CIRCLE_ATANH> : obj {
 //private:
  using obj::obj;
  using real = smns::defs::real;
  real x0{};
  real y0{};
  real phi{};
  real stretch{1.0};
  real x_scale{1.0};
  real y_scale{1.0};
  real phase = 0_r;
  real cos_phi = std::cos(phi);
  real sin_phi = std::sin(phi);

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

  virtual void calc_bounds() override;

  void set_phi(real phi);

  virtual void draw(window* win) override;
};
