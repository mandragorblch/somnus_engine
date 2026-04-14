#include "heart.h"

#include <cmath>
#include <cstdint>

#include "smns/math/analytics.h"
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

heart<HEART_TYPES::PARABOLA>::heart(Window* win, real x0, real y0, real phi, real stretch,
                                    real x_scale, real y_scale)
    : render_t(color_t<>({255, 100, 150}),
               x_scale, y_scale),
      x0(x0),
      y0(y0),
      phi(phi),
      stretch(stretch) {
  calc_bounds(win);
  
  //TODO do i need a ceil?
  SDF_map.resize(std::ceil((top_bound - bottom_bound) * win->_height));
  real dy = 1_r / win->_height;
  real y = top_bound;
  for (auto i = SDF_map.size(); i-- > 0; ) {
    auto dist_x = smns::analytic::newton_solver_const_y(
        [&](real x, real y) { return func(x, y); }, 0, y);
    if (dist_x <= 0) break;
    SDF_map[i] = map_to_screen_width(dist_x, win) + 1;
    y -= dy;
  }
}

void heart<HEART_TYPES::PARABOLA>::calc_bounds(Window* win) {
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
void heart<HEART_TYPES::PARABOLA>::draw(Window* window) {
  auto* surf = window->_surface;
  SDL_PixelFormat fmt = surf->format;
  auto details = SDL_GetPixelFormatDetails(fmt);
  check(details);
  auto* pixels = reinterpret_cast<uint32_t*>(surf->pixels);
  auto pitch = surf->pitch / sizeof(uint32_t);

  auto top_cropped_world = crop(top_bound + pos.y);
  auto bottom_cropped_world = crop(bottom_bound + pos.y);
  auto right_cropped_world = crop(right_bound + pos.x);
  auto left_cropped_world = crop(left_bound + pos.x);

  auto top_cropped = top_cropped_world - pos.y;
  auto bottom_cropped = bottom_cropped_world - pos.y;
  auto right_cropped = right_cropped_world - pos.x;
  auto left_cropped = left_cropped_world - pos.x;

  auto top_cropped_mapped_relative = map_to_screen_height(top_cropped, window);
  auto bottom_cropped_mapped_relative = map_to_screen_height(bottom_cropped, window);
  auto right_cropped_mapped_relative = map_to_screen_height(right_cropped, window);
  auto left_cropped_mapped_relative = map_to_screen_height(left_cropped, window);

  auto bottom_cropped_mapped_world =
      map_to_screen_height(bottom_cropped_world, window);
  auto top_cropped_mapped_world =
      map_to_screen_height(top_cropped_world, window);
  auto right_cropped_mapped_world =
      map_to_screen_width(right_cropped_world, window);
  auto left_cropped_mapped_world =
      map_to_screen_width(left_cropped_world, window);

  auto pos_cropped_mapped = map_to_screen_size(crop(pos), window);
  auto pos_mapped = map_to_screen_size(pos, window);

  auto r = std::abs(right_cropped);
  auto l = std::abs(left_cropped);
  auto min_v = std::min(r, l);
  auto max_v = std::max(r, l);

  int max_cropped_mapped = map_to_screen_width(max_v, window);

  real x_begin;
  if (right_cropped < 0)
    x_begin = -right_cropped;
  else if (left_cropped > 0)
    x_begin = left_cropped;
  else
    x_begin = 0;
  auto y = top_cropped;
  auto x = x_begin;

  int curr_pix_y = top_cropped_mapped_world;
    for (int curr_rel_pix_y = top_cropped_mapped_relative;
       curr_rel_pix_y > bottom_cropped_mapped_relative; --curr_rel_pix_y) {
    
    auto* row = &pixels[flip_y(curr_pix_y, window) * pitch + pos_cropped_mapped.x];
    --curr_pix_y;
    auto x_skip = SDF_map[curr_rel_pix_y - bottom_cropped_mapped_relative];
    x = x_begin + static_cast<real>(x_skip) / (window->_width - 1);
    bool was_inside = false;
    for (int curr_pix_x = x_skip;
         curr_pix_x < max_cropped_mapped; ++curr_pix_x) {

      auto F_x_y = func(x, y);

      x += window->dx;
      //check if its inside and if not, continue
      //if (F_x_y >= 0) {
      //  if (was_inside) {
      //    break;
      //  } else {
      //    continue;
      //  }
      //} else {
      //  was_inside = true;
      //}
      if (F_x_y >= 0) break;

      color_t res_color = color;
      res_color *= std::abs(std::sin(std::pow(F_x_y * 60, 2) + phase));
      auto bgra = res_color.get_bgra();

      //distance between current x and center x
      auto dist = curr_pix_x;
      //right half
      if (curr_pix_x + pos_mapped.x < window->_width) {
        row[dist] = bgra;
      }
      //left half
      auto left = pos_mapped.x - dist;
      if (left >= 0 && left < window->_width) {
        row[-dist] = bgra;
      }
    }
    y -= window->dy;
  }
  phase += 0.1;
}
#pragma endregion

#pragma region CIRCLE_ATANH

#pragma endregion
