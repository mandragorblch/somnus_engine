#include "Renderer.h"

#pragma region ANALYTICAL 
using real = smns::defs::real;

template <class render_obj_t>
real renderer<RENDER_TYPE::ANALYTICAL, render_obj_t>::func(real x, real y)
{ static_cast<render_obj_t*>(this)->func(x, y); }

template <class render_obj_t>
void renderer<RENDER_TYPE::ANALYTICAL, render_obj_t>::render(Window* window)
 { static_cast<render_obj_t*>(this)->draw(window); }

template <class render_obj_t>
void renderer<RENDER_TYPE::ANALYTICAL, render_obj_t>::calculate_bounds(Window* window)
 { static_cast<render_obj_t*>(this)->calc_bounds(window); }

#pragma endregion