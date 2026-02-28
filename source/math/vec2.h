#pragma once
#include <type_traits>
#include "math/defs.h"

//real defined in math/defs.h is default
template <typename real_t = smns::defs::real>
  requires(std::is_arithmetic_v<real_t>)
struct vec2 {
  using real = smns::defs::real;
  real_t x{};
  real_t y{};

  vec2() = default;
  ~vec2() = default;

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2(const vec2<real_t_other>& other) {
    x = other.x;
    y = other.y;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2(real_t_other x, real_t_other y) : x(x), y(y) {}

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2& operator+=(const vec2<real_t_other>& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2 operator+(const vec2<real_t_other>& other) {
    vec2 ans(*this);
    ans += other;
    return ans;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2 operator*=(real_t_other r) {
    x *= r;
    y *= r;
    return *this;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2 operator*(real_t_other r) {
    vec2 ans = *this;
    ans *= r;
    return ans;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2 operator+=(real_t_other r) {
    x += r;
    y += r;
    return *this;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2 operator+(real_t_other r) {
    vec2 ans = *this;
    ans += r;
    return ans;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2& operator=(const vec2<real_t_other>& other) {
    x = other.x;
    y = other.y;
    return *this;
  }

  template <typename real_t_other>
    requires(std::is_arithmetic_v<real_t_other>)
  vec2& operator=(real_t_other r) {
    x = r;
    y = r;
    return *this;
  }
};