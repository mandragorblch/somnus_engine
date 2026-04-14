#pragma once
#include "objects/heart.h"


class Scene {
 public:
	std::vector<Object*> objects{};

  std::vector<heart<HEART_TYPES::PARABOLA>*> hearts_parabola{};
	std::vector<heart<HEART_TYPES::CIRCLE_ASIN>*> hearts_asin{};


	public:
	void add_object(Object* object);
  void render(Window* window);

	//TODO add handles for user defined behaviour
void add_renderer(heart<HEART_TYPES::PARABOLA>* p_renderer) {
  hearts_parabola.push_back(p_renderer);
};
};
