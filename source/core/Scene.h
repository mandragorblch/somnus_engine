#pragma once
#include "objects/heart.h"

template<typename T>
struct renderer_storage;

template<>
struct renderer_storage<heart<HEART_TYPES::PARABOLA>> {
    static auto& get(auto& scene) {
        return scene.hearts_parabola;
    }
};

class Scene {
 public:
	std::vector<Object*> objects{};

  std::vector<heart<HEART_TYPES::PARABOLA>*> hearts_parabola{};
	std::vector<heart<HEART_TYPES::CIRCLE_ASIN>*> hearts_asin{};


	public:
	void add_object(Object* object);
  void render(Window* window);

	//TODO add handles for user defined behaviour
	/*🧠 Fix: generational handles (real engine way)
template<typename T>
struct Handle {
    uint32_t index;
    uint32_t generation;
};

Storage:

struct Slot {
    T value;
    uint32_t generation;
    bool alive;
};
✔️ access check
T* get(Handle<T> h) {
    auto& slots = storage<T>::get(*this);

    if (h.index >= slots.size()) return nullptr;

    auto& slot = slots[h.index];

    if (!slot.alive || slot.generation != h.generation)
        return nullptr;

    return &slot.value;
}

👉 now:

safe ✅
no dangling access ✅
engine controls lifetime ✅*/
//void add_renderer(heart<HEART_TYPES::PARABOLA>* p_renderer) {
//  hearts_parabola.push_back(p_renderer);
//};
//	auto h = world.create<heart<PARABOLA>>();
//
//if (auto* obj = world.get(h)) {
//    obj->cool_behavior();
//}

template<typename T>
void add_renderer(T* r) {
    renderer_storage<T>::get(*this).push_back(r);
}
};
