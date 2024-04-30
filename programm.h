#pragma once
#include "init_storage.h"
#include "PhysX/PhysX.h"
#include "somnus_engine/maths/math.h"
#include "somnus_engine/render_objects/animator.h"
#include "shapes/create_text_object.h"
#include "objects/types/types.h"
#include <thread>
#include <list>

class programm
{
private:
	class PhysX* p_PhysX = nullptr;

	class init_storage* p_storage;

	class vk* p_vk;

	std::vector<class two_d_object*> two_d_objects;
	std::vector<class button*> buttons;
	//std::vector<struct computeCubeHitbox> all_cube_hitboxes;
	void move(int index);
	std::vector<class model*> models;
	std::vector<class Animation*> animations;
	std::vector<class Animator*> animators;
	std::vector<class gameObject*>& gameObjects;
	class two_d_object *fps_counter;
public:
	programm(class init_storage* p_storage);
	~programm();
	void mainLoop();
};

