#pragma once
#include "World.h"
#include "objects/heart.h"

using RenderTypes =
    type_list<heart<HEART_TYPES::PARABOLA>*, heart<HEART_TYPES::CIRCLE_ASIN>*>;

class Scene {
	using real = smns::defs::real;
	 public:
  World<RenderTypes> world;

  void add_object(Object* object);
  void render(Window* window);
	void tick(real dt);

  template <typename render_t>
  auto& get() {
    return world.get<render_t>();
  }
};