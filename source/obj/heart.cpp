#include "heart.h"

#include <cmath>

#include "helpers/sdl_hlprs.h"

using namespace smns::sdl_hlprs;

heart::heart(real x0, real y0, real phi, real x_scale, real y_scale)
    : x0(x0), y0(y0), phi(phi), x_scale(x_scale), y_scale(y_scale) {
  calc_bounds();
}

void heart::calc_bounds() {
  real cos_phi = std::cos(phi);
  real sin_phi = std::sin(phi);
  real cos_phi_sq = cos_phi * cos_phi;
  real sin_phi_sq = sin_phi * sin_phi;
  real tan_phi = sin_phi / cos_phi;
  real cot_phi = 1_r / tan_phi;

  real mult = y_scale / (4_r * x_scale * x_scale);

  top_bound = y0 - mult * tan_phi * sin_phi;
  real a = y_scale * cos_phi;
  real b = 2_r * x_scale * x_scale * sin_phi;
  bottom_bound =
      (a - std::sqrt(a * a - 2_r * b * x0 * y_scale)) / (b * sin_phi) -
      x0 * cot_phi + y0;

  left_bound = x0 - mult * cot_phi * cos_phi;
  right_bound = -left_bound;
}

// the surface must be locked
void heart::draw(SDL_Surface* surf) {
  // TODO draw logic
  SDL_PixelFormat fmt = surf->format;
  auto details = SDL_GetPixelFormatDetails(fmt);
  check(details);

  Uint32 clr = SDL_MapRGB(details, NULL, 144, 12, 255);
  check(SDL_FillSurfaceRect(surf, NULL, clr));

  auto pixs = reinterpret_cast<Uint32*>(surf->pixels);
}
