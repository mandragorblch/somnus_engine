#pragma once
#include <cstdint>
#include <type_traits>
#include <algorithm>

struct color {
  uint8_t data[4] = {255, 255, 255, 255};
  uint8_t& r = reinterpret_cast<uint8_t&>(data[0]);
  uint8_t& g = reinterpret_cast<uint8_t&>(data[1]);
  uint8_t& b = reinterpret_cast<uint8_t&>(data[2]);
  uint8_t& alpha = reinterpret_cast<uint8_t&>(data[3]);

  uint8_t& operator[](size_t index) { return data[index]; }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  color operator*(real_t val) {
    return color{static_cast<uint8_t>(val * r), static_cast<uint8_t>(val * g),
                 static_cast<uint8_t>(val * b)};
  }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  color& operator*=(real_t val) {
    r *= val;
    g *= val;
    b *= val;
    return *this;
  }

  color operator+(const color& other) {
    return color{static_cast<uint8_t>(other.r + r),
                 static_cast<uint8_t>(other.g + g),
                 static_cast<uint8_t>(other.b + b)};
  }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  color operator+(real_t val) {
    return color{static_cast<uint8_t>(val + r),
                 static_cast<uint8_t>(val + g),
                 static_cast<uint8_t>(val + b)};
  }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  color operator+=(real_t val) {
    r += val;
    g += val;
    b += val;
    return *this;
  }

  color& operator=(const color& other) {
    reinterpret_cast<uint32_t&>(data[0]) =
        reinterpret_cast<const uint32_t&>(other.data[0]);
    return *this;
  }

  //clamped to 255
  template <typename real_t>
    requires(std::is_integral_v<real_t>)
  color& operator=(real_t val) {
    uint8_t ch_val = static_cast<uint8_t>(std::min(val, 255));
    r = ch_val;
    g = ch_val;
    b = ch_val;
    return *this;
  }

    //clamped to 1 and then turned to [0, 255] range
  template <typename real_t>
    requires(std::is_floating_point_v<real_t>)
  color& operator=(real_t val) {
    uint8_t ch_val = static_cast<uint8_t>(std::min(val, 1) * 255);
    r = ch_val;
    g = ch_val;
    b = ch_val;
    return *this;
  }

  void white() { *this = 255; }
  void black() { *this = 0; }
  void green() {
    r = 0;
    g = 255;
    b = 0;
  }
  void red() {
    r = 255;
    g = 0;
    b = 0;
  }
  void blue() {
    r = 0;
    g = 0;
    b = 255;
  }
  void purple() {
    r = 255;
    g = 0;
    b = 255;
  }
};