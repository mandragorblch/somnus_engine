#include "color.h"

template <>
color_base<PIXEL_FORMAT::RGBA_RGBA>::color_base(uint8_t r, uint8_t g, uint8_t b,
                                                uint8_t alpha)
    : data{r, g, b, alpha} {}

template <>
color_base<PIXEL_FORMAT::BGRA_RGBA>::color_base(uint8_t r, uint8_t g, uint8_t b,
                                                uint8_t alpha)
    : data{b, g, r, alpha} {}

