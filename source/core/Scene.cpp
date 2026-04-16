#include "Scene.h"
#include "../core/Window.h"

namespace smns::core{
void Scene::add_object(Object* object) {
  world.objects.push_back(object);
}

void Scene::render(Window* window) {
  auto& hearts_parabola = get<heart<HEART_TYPES::PARABOLA>*>();
  for (auto current_heart : hearts_parabola) {
    current_heart->draw(window);
  }
}

void Scene::tick(real dt) {
	//world.tick(dt);
}
}