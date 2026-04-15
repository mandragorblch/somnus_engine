#pragma once
#include "objects/heart.h"

template <typename... Ts>
struct type_list {};

template <typename data_t>
struct Storage {
  std::vector<data_t> data;
};

template <typename List>
struct World;

template <typename... Ts>
struct World<type_list<Ts...>> {
	using real = smns::defs::real;
  // TODO make engine own objects
  // remove all references and so on
 public:
  std::tuple<Storage<Ts>...> storages;
  std::vector<Object*> objects{};

  template <typename render_t>
  auto& get();

 public:
  void add_object(Object* object);
  void render(Window* window);
	void tick(real dt);

  // TODO add handles for user defined behaviour (handles)

  template <typename render_t>
  void add_render_object(render_t render_object) {
    get<render_t>().push_back(render_object);
  }
};

template <typename... Ts>
template <typename render_t>
inline auto& World<type_list<Ts...>>::get() {
  return std::get<Storage<render_t>>(storages).data;
}


template <typename... Ts>
inline void World<type_list<Ts...>>::add_object(Object* object) {
	objects.push_back(object);
}

template <typename... Ts>
inline void World<type_list<Ts...>>::render(Window* window) {
	auto& hearts_parabola = get<heart<HEART_TYPES::PARABOLA>*>();
  for (auto current_heart : hearts_parabola) {
    current_heart->draw(window);
  }
}

template <typename... Ts>
inline void World<type_list<Ts...>>::tick(real dt) {
	for(size_t i = 0; i < objects.size();) {
		Object& object = *objects[i];
		object.vel += object.force * dt;
		object.pos += object.vel * dt;
		object.vel *= 0.99;
		auto& hearts = get<heart<HEART_TYPES::PARABOLA>*>();

		if (std::abs(object.pos.x) > 10 || std::abs(object.pos.y) > 10) {
			delete objects[i];
			objects.erase(objects.begin() + i);
			auto& hearts = get<heart<HEART_TYPES::PARABOLA>*>();
			hearts.erase(hearts.begin() + i);
			continue;
		}
		++i;
	}
}
