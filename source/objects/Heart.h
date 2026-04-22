#pragma once
#include "smns/defs.h"
#include "objects/base_objects/Renderer.h"
#include <functional>

enum class HEART_TYPES : uint8_t {
  PARABOLA,
  CIRCLE_ATANH,
  CIRCLE_SQRT,
  CIRCLE_LINE,
  CIRCLE_ASIN,
  CIRCLE_PARABOLA
};

// the heart object
// defines as rotated by an angle and mirrored parabola
template<HEART_TYPES type>
class heart : public Renderable<RENDER_TYPE::ANALYTICAL, heart<type>> {};

template<>
class heart<HEART_TYPES::PARABOLA> : public Renderable<RENDER_TYPE::ANALYTICAL, heart<HEART_TYPES::PARABOLA>> {
 public:
  using real = smns::defs::real;
  using render_t = Renderable<RENDER_TYPE::ANALYTICAL, heart<HEART_TYPES::PARABOLA>>;
  using render_t::render_t;
  real x0{};
  real y0{};

 private:
  real phi{};

 public:
  real stretch{1.0};
  real phase{};
  real cos_phi = std::cos(phi);
  real sin_phi = std::sin(phi);
	//first non-zero entry in row of SDF_map
  std::vector<size_t> SDF_first{};
	//last non-zero entry in row of SDF_map
	std::vector<size_t> SDF_last{};
	std::vector<std::vector<real>> SDF_map{}; 
  

  real func(real x, real y);

  // constructor
  heart(Window* win, real x0, real y0, real phi, real stretch = real{1.0}, real x_scale = real{1.0}, real y_scale = real{1.0});

  // copy constructor
  heart(const heart&) = default;

  // copy assignment
  heart& operator=(const heart&) = default;

  // move constructor
  heart(heart&&) noexcept = default;

  // move assignment
  heart& operator=(heart&&) noexcept = default;

  // destructor
  ~heart() = default;

  void calc_bounds(int window_width, int window_height);

  void set_phi(real phi);

  void draw(Window* window);
};