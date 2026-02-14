#include "color.h"
#include <bit>

template struct color<PIXEL_FORMAT::BGRA_RGBA>;
template struct color<PIXEL_FORMAT::RGBA_RGBA>; 




template <PIXEL_FORMAT frmt>
color_base<frmt>::color_base(const color_base& other) : data(other.data) {

}

template <PIXEL_FORMAT frmt>
color<frmt>::color(const color_base<frmt>& base)
    : color_base<frmt>(base) 
{}


template <>
color_base<PIXEL_FORMAT::RGBA_RGBA>::color_base(uint8_t r, uint8_t g, uint8_t b,
                                                uint8_t a)
    : data(r, g, b, a) {}

uint32_t color<PIXEL_FORMAT::RGBA_RGBA>::get_rgba() {
  uint32_t res = std::bit_cast<uint32_t>(data);
  return res;
}

uint32_t color<PIXEL_FORMAT::RGBA_RGBA>::get_bgra() {
    uint32_t res = std::byteswap(get_rgba());
    return std::rotl(res, 8);
  }



template <>
color_base<PIXEL_FORMAT::BGRA_RGBA>::color_base(uint8_t r, uint8_t g, uint8_t b,
                                                uint8_t a)
    : data(b, g, r, a) {}

uint32_t color<PIXEL_FORMAT::BGRA_RGBA>::get_bgra() {
  uint32_t res = std::bit_cast<uint32_t>(data);
  return res;
}

uint32_t color<PIXEL_FORMAT::BGRA_RGBA>::get_rgba()  {
    uint32_t res = std::byteswap(get_bgra());
    return std::rotl(res, 8);
  }
