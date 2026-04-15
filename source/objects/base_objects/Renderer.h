#pragma once
#include "smns/defs.h"
#include "Object.h"
#include "visual/color.h"
#include <functional>

enum class RENDER_TYPE : uint8_t { IMAGE, ANALYTICAL };

class Renderable_base {
  using real = smns::defs::real;

 public:
  real x_scale{1.0};
  real y_scale{1.0};
  real top_bound{1.0};
  real bottom_bound{1.0};
  real left_bound{1.0};
  real right_bound{1.0};

  int top_bound_pix;
  int bottom_bound_pix;
  int left_bound_pix;
  int right_bound_pix;
};

template <RENDER_TYPE render_t, class render_obj_t>
class Renderable : public Object, public Renderable_base {};

template <class render_obj_t>
class Renderable<RENDER_TYPE::ANALYTICAL, render_obj_t> : public Object,
                                                        public Renderable_base {
  using real = smns::defs::real;
  using render_t = Renderable<RENDER_TYPE::ANALYTICAL, render_obj_t>;
  using Renderable_base::Renderable_base;

 public:
  Color<> color{};
  //implicit function
  real func(real x, real y);

  Renderable() = delete;
  Renderable(Color<> color, real x_scale = real{1.0},
           real y_scale = real{1.0})
      : Renderable_base({x_scale, y_scale}), color(color) {}
  Renderable(const render_t& other) = default;
  render_t& operator=(const render_t& other) = default;
  Renderable(render_t&& other) = default;
  render_t& operator=(render_t&& other) = default;

  // TODO math for calculating relative positions of existing windows
  void render(Window* window);
  void calculate_bounds(Window* window);
};