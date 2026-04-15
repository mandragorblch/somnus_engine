#pragma once
#include <SDL3/SDL.h>

#include "smns/defs.h"
#include "smns/math/vec2.h"

class Window;

#pragma region virtual_implementation
class Object {
 public:
  using real = smns::defs::real;
  struct vec2<> pos{};
  struct vec2<> vel{};
  real mass{1.0};



  Object(vec2<> pos = vec2<>{},
      vec2<> vel = vec2<>{}, real mass = real{1.0});
  Object(const Object&) = default;
  Object& operator=(const Object&) = default;
  Object(Object&&) = default;
  Object& operator=(Object&&) = default;

  // TODO math for calculating relative positions of existing windows
};
#pragma endregion


//CRTP
//no way if i wanna write foreach curr_obj->draw()
//but do i really need that???
//template<class render_t>
//class Object {
// public:
//  Color<> color{};
//  vec2<> pos{};
//  vec2<> vel{};
//  real mass{1.0};
//
//  Object(const Color<>& Color = Color<>{}, vec2<> pos = vec2<>{},
//      vec2<> vel = vec2<>{}, real mass = real{1.0});
//  Object(const Object&) = default;
//  Object& operator=(const Object&) = default;
//  Object(Object&&) = default;
//  Object& operator=(Object&&) = default;
//};
