#pragma once
#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <array>
#include <iostream>

// first format - how it stored
// second - how it being treated
// BGRA_RGBA - stored like BGRA, but u put colors in RGBA
enum class PXL_FRMT : uint8_t { RGBA_RGBA, BGRA_RGBA };

enum class PXL_CHNL : uint8_t { RED, GREEN, BLUE, ALPHA };

template<PXL_FRMT frmt>
struct pixel_map;

template<>
struct pixel_map<PXL_FRMT::BGRA_RGBA> {
    using r_indx = std::integral_constant<size_t, 2>;
    using g_indx = std::integral_constant<size_t, 1>;
    using b_indx = std::integral_constant<size_t, 0>;
    using a_indx = std::integral_constant<size_t, 3>;
};

template<>
struct pixel_map<PXL_FRMT::RGBA_RGBA> {
    using r_indx = std::integral_constant<size_t, 0>;
    using g_indx = std::integral_constant<size_t, 1>;
    using b_indx = std::integral_constant<size_t, 2>;
    using a_indx = std::integral_constant<size_t, 3>;
};



// white by default
template <PXL_FRMT frmt = PXL_FRMT::BGRA_RGBA>
struct Color {
  std::array<uint8_t, 4> data;

  //for gradient latches
  //false - rising, true - decreasing
  std::array<bool, 4> gradient_latches{false, false, false, false};

  template<PXL_CHNL chnl_indx>
  static constexpr size_t indx_of = []() {
    // 'if constexpr' discards the non-matching branches at compile-time.
    // No runtime cost. Pure constant folding.
    if constexpr (chnl_indx == PXL_CHNL::RED) 
        return pixel_map<frmt>::r_indx::value;
    else if constexpr (chnl_indx == PXL_CHNL::GREEN) 
        return pixel_map<frmt>::g_indx::value;
    else if constexpr (chnl_indx == PXL_CHNL::BLUE) 
        return pixel_map<frmt>::b_indx::value;
    else if constexpr (chnl_indx == PXL_CHNL::ALPHA) 
        return pixel_map<frmt>::a_indx::value;
    else 
        return size_t(-1); // Safety fallback
  }(); // <--- Note the parenthesis! We call the lambda immediately.

  auto r_ind() { return indx_of<PXL_CHNL::RED>; }
  auto g_ind() { return indx_of<PXL_CHNL::GREEN>; }
  auto b_ind() { return indx_of<PXL_CHNL::BLUE>; }
  auto a_ind() { return indx_of<PXL_CHNL::ALPHA>; }

  uint8_t& r() { return data[r_ind()]; };
  uint8_t& g() { return data[g_ind()]; };
  uint8_t& b() { return data[b_ind()]; };
  uint8_t& a() { return data[a_ind()]; };

  //sets all channels to 255 by default
  Color(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255,
             uint8_t alpha = 255);

  uint32_t get_rgba();
  uint32_t get_bgra();

  //yes, template. questions?
  //makes smooth gradient from 0 to 255 and back
  //chnl_ind - r/g/b/a channel
  //use PXL_CHNL as chnl_indx
  template<PXL_CHNL chnl>
  void gradient_step() {
    auto chnl_ind = indx_of<chnl>;
    bool& latch = gradient_latches[chnl_ind];
    auto& cur_col = data[chnl_ind];
    if (latch) {
      --cur_col;
      if (cur_col == 0) {
        latch = false;
      }
    } else {
      ++cur_col;
      if (cur_col == 255) {
        latch = true;
      }
    }
  }



  uint8_t& operator[](size_t index) { return data[index]; }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  Color operator*(real_t val) {
    return Color{static_cast<uint8_t>(val * data[0]),
                      static_cast<uint8_t>(val * data[1]),
                      static_cast<uint8_t>(val * data[2])};
  }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  Color& operator*=(real_t val) {
    data[0] = static_cast<uint8_t>(data[0] * val);
    data[1] = static_cast<uint8_t>(data[1] * val);
    data[2] = static_cast<uint8_t>(data[2] * val);
    return *this;
  }

  Color operator+(const Color& other) {
    return Color{static_cast<uint8_t>(other.data[0] + data[0]),
                      static_cast<uint8_t>(other.data[1] + data[1]),
                      static_cast<uint8_t>(other.data[2] + data[2])};
  }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  Color operator+(real_t val) {
    return Color{static_cast<uint8_t>(val + data[0]),
                      static_cast<uint8_t>(val + data[1]),
                      static_cast<uint8_t>(val + data[2])};
  }

  template <typename real_t>
    requires(std::is_arithmetic_v<real_t>)
  Color operator+=(real_t val) {
    data[0] += val;
    data[1] += val;
    data[2] += val;
    return *this;
  }

  Color& operator=(const Color& other) {
    reinterpret_cast<uint32_t&>(data[0]) =
        reinterpret_cast<const uint32_t&>(other.data[0]);
    return *this;
  }

  // clamped to 255
  template <typename real_t>
    requires(std::is_integral_v<real_t>)
  Color& operator=(real_t val) {
    uint8_t ch_val = static_cast<uint8_t>(std::min(static_cast<uint8_t>(val), static_cast<uint8_t>(255)));
    data[0] = ch_val;
    data[1] = ch_val;
    data[2] = ch_val;
    return *this;
  }

  // clamped to 1 and then turned to [0, 255] range
  template <typename real_t>
    requires(std::is_floating_point_v<real_t>)
  Color& operator=(real_t val) {
    uint8_t ch_val = static_cast<uint8_t>(std::min(val, 1) * 255);
    data[0] = ch_val;
    data[1] = ch_val;
    data[2] = ch_val;
    return *this;
  }

  void white() { *this = 255; }
  void black() { *this = 0; }
  void green() {
    data[0] = 0;
    data[1] = 255;
    data[2] = 0;
  }
  void red() {
    data[0] = 255;
    data[1] = 0;
    data[2] = 0;
  }
  void blue() {
    data[0] = 0;
    data[1] = 0;
    data[2] = 255;
  }
  void purple() {
    data[0] = 255;
    data[1] = 0;
    data[2] = 255;
  }
};