#include "heart.h"

#include <cmath>
#include <cstdint>

#include "helpers/sdl_hlprs.h"
#include "smns/math/analytics.h"

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

heart<HEART_TYPES::PARABOLA>::heart(Window* win, real x0, real y0, real phi,
                                    real stretch, real x_scale, real y_scale)
    : render_t(Color<>({255, 100, 150}), x_scale, y_scale),
      x0(x0),
      y0(y0),
      phi(phi),
      stretch(stretch) {
  int window_height = win->_height;
  int window_width = win->_width;

  calc_bounds(window_width, window_height);
  // include bottom bound too
  size_t height_pix = top_bound_pix - bottom_bound_pix;
  SDF_first.resize(height_pix + 1);
	SDF_last.resize(height_pix + 1);
  real dy = win->dy;
  real y = top_bound;

	//TODO decide what is benificial - find first entries by newton or exact by just filling SDF map
	//TODO put all that into a function to rebuild when parametr changes
	////find first non-zero entries
 // for (auto i = height_pix; i-- > 0;) {
 //   auto dist_x = smns::analytic::newton_solver_const_y(
 //       [&](real x, real y) { return func(x, y); }, 0, y, 10);
 //   if (dist_x <= 0) break;
 //   SDF_first[i] = dist_x;
 //   y -= dy;
 // }

	////find last non-zero entries
 // y = top_bound;
 // for (auto i = height_pix; i-- > 0;) {
 //   auto dist_x = smns::analytic::newton_solver_const_y(
 //       [&](real x, real y) { return func(x, y); }, right_bound, y, 10);
 //   if (dist_x <= 0) break;
 //   SDF_last[i] = dist_x;
 //   y -= dy;
 // }

	// calculate SDF map
	auto epsilon = 2_r * std::sqrt(std::numeric_limits<real>::epsilon());
	auto dx = win->dx;
  y = bottom_bound;
	real x; 
	//TODO think if i really need + 1????
  SDF_map.resize(right_bound_pix + 1);
	for(auto& column : SDF_map) {
    column.resize(height_pix + 1);
  }

  for (size_t y_pix = 0; y_pix <= height_pix; ++y_pix, y += dy) {
		int x_pix = 0;
		x = 0;
		auto F_x_y = func(x, y);
		if(F_x_y > epsilon) {
			do {
				x += dx;
				++x_pix;
				F_x_y = func(x, y);
				//TODO ensure that there is always at least 1 entry
			} while (F_x_y > epsilon && x_pix <= right_bound_pix);
			SDF_map[x_pix - 1][y_pix] = F_x_y;
			SDF_first[y_pix] = x_pix - 1;
		}
    for (; x_pix <= right_bound_pix; ++x_pix, x += dx) {
      F_x_y = func(x, y);
			if(F_x_y > epsilon) {
				break;
			}
			SDF_map[x_pix][y_pix] = F_x_y;
    }
		SDF_last[y_pix] = x_pix;
  }
  //for (size_t Y = 0; Y <= height_pix; ++Y) {
  //  for (size_t X = 0; X <= right_bound_pix; ++X) {
  //    std::cout << std::setw(3) << (std::abs(SDF_map[X][Y]) > 0);
  //  }
  //  std::cout << '\n';
//  }
}

//TODO consider left and bottom bounds to be 0
//TODO guarantee that there is no empty rows/cols
void heart<HEART_TYPES::PARABOLA>::calc_bounds(int window_width,
                                               int window_height) {
	//just formulas
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

  top_bound_pix = map_to_window_height(top_bound, window_height);
  bottom_bound_pix = map_to_window_height(bottom_bound, window_height);
  right_bound_pix = map_to_window_width(right_bound, window_height);
  left_bound_pix = -right_bound_pix;
}

void heart<HEART_TYPES::PARABOLA>::set_phi(real phi) {
  this->phi = phi;
  cos_phi = std::cos(phi);
  sin_phi = std::sin(phi);
}

//render for dynamic parametrs
////TODO two different renders for the objects that changes params every frame/treat SDF like a picture
//// the surface must be locked
//void heart<HEART_TYPES::PARABOLA>::draw(Window* window) {
//  auto* surf = window->_surface;
//  SDL_PixelFormat fmt = surf->format;
//  auto details = SDL_GetPixelFormatDetails(fmt);
//  check(details);
//  auto* pixels = reinterpret_cast<uint32_t*>(surf->pixels);
//  auto pitch = surf->pitch / sizeof(uint32_t);
//  auto window_width = window->_width;
//  auto window_height = window->_height;
//
//  auto pos_pix_crop_world =
//      map_to_window_size(crop(pos), window_width, window_height);
//  auto pos_pix_world = map_to_window_size(pos, window_width, window_height);
//
//  auto y_max_world = crop(top_bound + pos.y);
//  auto y_min_world = crop(bottom_bound + pos.y);
//  auto x_max_world = crop(right_bound + pos.x);
//  auto x_min_world = crop(left_bound + pos.x);
//
//  auto y_max = y_max_world - pos.y;
//  auto y_min = y_min_world - pos.y;
//  auto x_max = x_max_world - pos.x;
//  auto x_min = x_min_world - pos.x;
//
//  auto y_max_pix_world =
//      crop_to_window_height(top_bound_pix + pos_pix_world.y, window_height);
//  auto y_min_pix_world =
//      crop_to_window_height(bottom_bound_pix + pos_pix_world.y, window_height);
//  auto x_max_pix_world =
//      crop_to_window_width(right_bound_pix + pos_pix_world.x, window_width);
//  auto x_min_pix_world =
//      crop_to_window_width(left_bound_pix + pos_pix_world.x, window_width);
//
//  auto y_max_pix = y_max_pix_world - pos_pix_world.y;
//  auto y_min_pix = y_min_pix_world - pos_pix_world.y;
//  auto x_max_pix = x_max_pix_world - pos_pix_world.x;
//  auto x_min_pix = x_min_pix_world - pos_pix_world.x;
//
//  auto r = std::abs(x_max);
//  auto l = std::abs(x_min);
//  auto min_v = std::min(r, l);
//  auto max_v = r + l - min_v;
//
//  int max_cropped_mapped = map_to_window_width(max_v, window_width);
//  int min_cropped_mapped = map_to_window_width(min_v, window_width);
//
//  // center out of window
//  bool coow = x_max < 0 || x_min > 0;
//  real x_begin = min_v * coow;
//  auto x = x_begin;
//  auto y = y_max;
//  auto dx = window->dx;
//  auto dy = window->dy;
//  auto epsilon = 2 * std::sqrt(std::numeric_limits<real>::epsilon());
//
//  int curr_y_pix_world = y_max_pix_world;
//  for (int curr_rel_pix_y = y_max_pix; curr_rel_pix_y >= y_min_pix;
//       --curr_rel_pix_y, --curr_y_pix_world, y -= dy) {
//    auto* row = &pixels[flip_y(curr_y_pix_world, window_height) * pitch];
//    // TODO x_skip goes from centre, we should substract distance between centre
//    // and how much of heart left on window
//		auto y_pix = curr_rel_pix_y - y_min_pix;
//    real x_border = SDF_map[SDF_first[y_pix]][y_pix];
//
//    x = std::max(x_begin, x_border);
//    auto x_border_pix = map_to_window_width(x, window_width);
//
//    auto F_x_y = func(x, y);
//    while (F_x_y > epsilon && x_border_pix <= max_cropped_mapped) {
//      x += dx;
//      ++x_border_pix;
//      F_x_y = func(x, y);
//    }
//    for (int curr_pix_x = x_border_pix; curr_pix_x <= max_cropped_mapped;
//         ++curr_pix_x, x += dx, F_x_y = func(x, y)) {
//      if (F_x_y > epsilon) {
//        break;
//      }
//
//      Color res_color = color;
//			auto aboba = F_x_y * 60;
//      res_color *= std::abs(std::sin(aboba * aboba + phase));
//			//stretch += 0.00001 * std::sin(phase / 10);
//      auto bgra = res_color.get_bgra();
//
//      // distance between current x and center x
//      auto dist = curr_pix_x;
//      auto right = pos_pix_world.x + dist;
//      // right half
//      if (right >= 0 && right < window_width) {
//        row[right] = bgra;
//      }
//      // left half
//      auto left = pos_pix_world.x - dist;
//      if (left >= 0 && left < window_width) {
//        row[left] = bgra;
//      }
//    }
//    //  SDL_UnlockSurface(surf);
//    // check(SDL_UpdateWindowSurface(window->_window));
//  }
//  phase += 0.1;
//}

//TODO SDF like a picture
// the surface must be locked
//TODO its not optimized for some reason (enough for today)
void heart<HEART_TYPES::PARABOLA>::draw(Window* window) {
  auto* surf = window->_surface;
  SDL_PixelFormat fmt = surf->format;
  auto details = SDL_GetPixelFormatDetails(fmt);
  check(details);
  auto* pixels = reinterpret_cast<uint32_t*>(surf->pixels);
  auto pitch = surf->pitch / sizeof(uint32_t);
  auto window_width = window->_width;
  auto window_height = window->_height;

  auto pos_pix_crop_world =
      map_to_window_size(crop(pos), window_width, window_height);
  auto pos_pix_world = map_to_window_size(pos, window_width, window_height);

  auto y_max_world = crop(top_bound + pos.y);
  auto y_min_world = crop(bottom_bound + pos.y);
  auto x_max_world = crop(right_bound + pos.x);
  auto x_min_world = crop(left_bound + pos.x);

  auto y_max = std::clamp(y_max_world - pos.y, 0_r, top_bound);
  auto y_min = std::clamp(y_min_world - pos.y, bottom_bound, 0_r);
  auto x_max = std::clamp(x_max_world - pos.x, 0_r, right_bound);
  auto x_min = std::clamp(x_min_world - pos.x, left_bound, 0_r);

  auto y_max_pix_world =
      crop_to_window_height(top_bound_pix + pos_pix_world.y, window_height);
  auto y_min_pix_world =
      crop_to_window_height(bottom_bound_pix + pos_pix_world.y, window_height);
  auto x_max_pix_world =
      crop_to_window_width(right_bound_pix + pos_pix_world.x, window_width);
  auto x_min_pix_world =
      crop_to_window_width(left_bound_pix + pos_pix_world.x, window_width);

  auto y_max_pix = std::clamp(y_max_pix_world - pos_pix_world.y, 0, top_bound_pix);
  auto y_min_pix = std::clamp(y_min_pix_world - pos_pix_world.y, bottom_bound_pix, 0);
  auto x_max_pix = std::clamp(x_max_pix_world - pos_pix_world.x, 0, right_bound_pix);
  auto x_min_pix = std::clamp(x_min_pix_world - pos_pix_world.x, left_bound_pix, 0);

  auto r = std::abs(x_max);
  auto l = std::abs(x_min);
  auto min_v = std::min(r, l);
  auto max_v = r + l - min_v;

  int max_cropped_mapped = map_to_window_width(max_v, window_width);
  int min_cropped_mapped = map_to_window_width(min_v, window_width);

  // center out of window
  bool coow = x_max < 0 || x_min > 0;
  real x_begin = min_v * coow;
  auto x = x_begin;
  auto y = y_max;
  auto dx = window->dx;
  auto dy = window->dy;

  int curr_y_pix_world = y_max_pix_world;
  for (int curr_rel_pix_y = y_max_pix; curr_rel_pix_y >= y_min_pix;
       --curr_rel_pix_y, --curr_y_pix_world, y -= dy) {
    auto* row = &pixels[flip_y(curr_y_pix_world, window_height) * pitch];
    // TODO x_skip goes from centre, we should substract distance between centre
    // and how much of heart left on window
		auto y_pix = curr_rel_pix_y - y_min_pix;
		auto x_first_pix = SDF_first[y_pix];
    real x_first = map_to_window_relative_width(x_first_pix, window_width);

    x = std::max(x_begin, x_first);
    auto x_border_pix = map_to_window_width(x, window_width);
    for (int curr_pix_x = x_first_pix; curr_pix_x <= max_cropped_mapped;
         ++curr_pix_x, x += dx) {
			auto F_x_y = SDF_map[curr_pix_x][y_pix];
      if (F_x_y == 0) {
        break;
      }

      Color res_color = color;
			auto aboba = F_x_y * 60;
      res_color *= std::abs(std::sin(aboba * aboba + phase));
			//stretch += 0.00001 * std::sin(phase / 10);
      auto bgra = res_color.get_bgra();

      // distance between current x and center x
      auto dist = curr_pix_x;
      auto right = pos_pix_world.x + dist;
      // right half
      if (right >= 0 && right < window_width) {
        row[right] = bgra;
      }
      // left half
      auto left = pos_pix_world.x - dist;
      if (left >= 0 && left < window_width) {
        row[left] = bgra;
      }
    }
    //  SDL_UnlockSurface(surf);
    // check(SDL_UpdateWindowSurface(window->_window));
  }
  phase += 0.1;
}
#pragma endregion

#pragma region CIRCLE_ATANH

#pragma endregion
