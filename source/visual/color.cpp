#include "color.h"
#include <bit>

template <>
color<PXL_FRMT::RGBA_RGBA>::color(uint8_t red, uint8_t green,
                                                uint8_t blue, uint8_t alpha)
    : data{red, green, blue, alpha} {}

template <>
color<PXL_FRMT::BGRA_RGBA>::color(uint8_t red, uint8_t green,
                                                uint8_t blue, uint8_t alpha)
    : data{blue, green, red, alpha} {
}

uint32_t color<PXL_FRMT::RGBA_RGBA>::get_rgba() {
  uint32_t res = std::bit_cast<uint32_t>(data);
  return res;
}

uint32_t color<PXL_FRMT::RGBA_RGBA>::get_bgra() {
  uint32_t res = std::byteswap(get_rgba());
  return std::rotl(res, 8);
}

uint32_t color<PXL_FRMT::BGRA_RGBA>::get_bgra() {
  uint32_t res = std::bit_cast<uint32_t>(data);
  return res;
}

uint32_t color<PXL_FRMT::BGRA_RGBA>::get_rgba() {
  uint32_t res = std::byteswap(get_bgra());
  return std::rotl(res, 8);
}