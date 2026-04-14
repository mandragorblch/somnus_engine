#include "Scene.h"

void Scene::render(Window* window) {
  for (auto current_heart : hearts_parabola) {
    current_heart->draw(window);
  }
}

void Scene::add_object(Object* object) { objects.push_back(object); }