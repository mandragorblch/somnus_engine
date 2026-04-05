#include "heart.h"

#include <cmath>
#include <cstdint>
#include "math/analytics.h"

#include "helpers/sdl_hlprs.h"

using namespace smns::defs::literals;
using namespace smns::sdl_hlprs;
using real = smns::defs::real;

#pragma region PARABOLA
real heart<HEART_TYPES::PARABOLA>::func(real x, real y) { 
      auto a = (x - x0) * cos_phi - (y - y0) * sin_phi;
      a *= a * stretch;
      auto b = (x - x0) * sin_phi + (y - y0) * cos_phi;
      return a - b;
}

heart<HEART_TYPES::PARABOLA>::heart(real x0, real y0, real phi, real stretch,
                                    real x_scale, real y_scale)
    : render_t(color<>({255, 100, 150}),
               x_scale, y_scale),
      x0(x0),
      y0(y0),
      phi(phi),
      stretch(stretch) {
  calc_bounds();
}

void heart<HEART_TYPES::PARABOLA>::calc_bounds() {
  real cos_phi_sqrd = cos_phi * cos_phi;
  real sin_phi_sqrd = sin_phi * sin_phi;

  real mult = 0.25_r / stretch;

  top_bound = y0 - mult * sin_phi_sqrd / cos_phi;
  real a = 2_r * stretch * sin_phi;
  bottom_bound =
      (cos_phi - std::sqrt(cos_phi_sqrd - 2_r * x0 * a)) / (a * sin_phi) -
      x0 * cos_phi / sin_phi + y0;

  right_bound = x0 - mult * cos_phi_sqrd / sin_phi;
  left_bound = -right_bound;
}

void heart<HEART_TYPES::PARABOLA>::set_phi(real phi) {
  this->phi = phi;
  cos_phi = std::cos(phi);
  sin_phi = std::sin(phi);
}

// the surface must be locked
void heart<HEART_TYPES::PARABOLA>::draw(window* win) {
  // TODO draw logic
  auto* surf = win->_surface;
  SDL_PixelFormat fmt = surf->format;
  auto details = SDL_GetPixelFormatDetails(fmt);
  check(details);
  auto* pixls = reinterpret_cast<uint32_t*>(surf->pixels);

  // auto top_bound_croped     = crop_to_screen(top_bound + pos.y);
  // auto bottom_bound_croped  = crop_to_screen(bottom_bound + pos.y);
  // auto right_bound_croped   = crop_to_screen(right_bound + pos.x);
  // auto left_bound_croped    = crop_to_screen(left_bound + pos.x);

  auto top_bound_cropped = crop_to_screen(top_bound + pos.y);
  auto bottom_bound_cropped = crop_to_screen(bottom_bound + pos.y);
  auto right_bound_cropped = crop_to_screen(right_bound + pos.x);
  bool right_is_cropped = right_bound_cropped < right_bound + pos.x;
  auto left_bound_cropped = crop_to_screen(left_bound + pos.x);
  bool left_is_cropped = left_bound_cropped > left_bound + pos.y;

  // RENDERS GO UPSIDE DOWN
  int bottom_bound_cropped_mapped =
      map_to_screen_height(top_bound_cropped, win);
  int top_bound_cropped_mapped =
      map_to_screen_height(bottom_bound_cropped, win);
  int right_bound_cropped_mapped =
      map_to_screen_width(right_bound_cropped, win);
  int left_bound_cropped_mapped = map_to_screen_width(left_bound_cropped, win);

  auto pos_cropped = crop_to_screen(pos);
  auto pos_mapped = map_to_screen(pos, win);
  auto pos_cropped_mapped = map_to_screen(pos_cropped, win);

  //TODO remake, does not have any mathematical sense, because its in absolute position
  //make bounds check in relative coords
  auto max_bound_cropped =
      std::max(right_bound_cropped, std::abs(left_bound_cropped - pos.x) + pos.x);
  auto max_bound_cropped_mapped = map_to_screen_width(max_bound_cropped, win);

  //TODO optimize
   for (auto curr_pix_x = max_bound_cropped_mapped;
       curr_pix_x-- > pos_cropped_mapped.x; ) {
    real rel_x = map_to_screen_relative_width(curr_pix_x, win) - pos.x;
    for (auto curr_pix_y = bottom_bound_cropped_mapped;
         curr_pix_y < top_bound_cropped_mapped; ++curr_pix_y) {
      real rel_y = map_to_screen_relative_height(curr_pix_y, win) - pos.y;
      auto F_x_y = func(rel_x, rel_y);
      bool is_inside = func(rel_x, rel_y) < 0;
      if (is_inside) {
        color res_color = clr;
         res_color *= std::abs(std::sin(std::pow(F_x_y * 60, 2) + phase));
        //auto left_mirror = map_to_screen_width(-rel_x + pos.x, win);
        //remap to the center of symmetry, then mirror and then back to pos_px
        auto rel_x_px_right = curr_pix_x - pos_mapped.x;
        auto rel_x_px_left = -rel_x_px_right;
        auto left_mirror = rel_x_px_left + pos_mapped.x;
        if (curr_pix_x < win->_width) {
          pixls[curr_pix_y * (surf->pitch / 4) + curr_pix_x] =
              res_color.get_bgra();
        }
        if (left_mirror >= 0 && left_mirror < win->_width) {
          pixls[curr_pix_y * (surf->pitch / 4) + left_mirror] = res_color.get_bgra();
        }
      }
    }
  }
  phase += 0.1;
}
#pragma endregion

#pragma region CIRCLE_ATANH

#pragma endregion
