#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "core/window.h"
#include "smns/defs.h"
#include "smns/math/vec2.h"

class Audio;

template <typename... msg_ts>
void out(msg_ts&&... args) {
  (std::cout << ... << args) << '\n';
}

namespace smns::sdl_hlprs {

  //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
int map_to_screen_width(num_t rel_coord, Window* win) {
  int res = static_cast<int>(std::floor((win->_width - 1) * rel_coord));
  return res;
}

 //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
int map_to_screen_height(num_t rel_coord, Window* win) {
  int res = static_cast<int>(std::floor((win->_height - 1) * rel_coord));
  return res;
}

//flip the coord
 //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
::smns::defs::real flip_relative(num_t rel_coord) {
  return 1_r - rel_coord;
}

//flip the coord
 //returns coords in pixels
int flip_y(int pix_coord, Window* win);

//flip the coord
 //returns coords in pixels
int flip_x(int pix_coord, Window* win);

 //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
int map_to_screen_height_flip(num_t rel_coord, Window* win) {
  int res = static_cast<int>((win->_height - 1) * (1_r - rel_coord));
  return res;
}

  //returns coords in pixels with y flipped (it renders upside down)
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
vec2<int> map_to_screen(const vec2<num_t>& rel_coords, Window* win) {
  vec2<int> res = {map_to_screen_width(rel_coords.x, win), map_to_screen_height_flip(rel_coords.y, win)};
  return res;
}

 //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
vec2<int> map_to_screen_size(const vec2<num_t>& rel_coords, Window* win) {
  vec2<int> res = {map_to_screen_width(rel_coords.x, win), map_to_screen_height_flip(rel_coords.y, win)};
  return res;
}

 //returns coord
::smns::defs::real map_to_screen_relative_height_flip(int pix_coord, Window* win);

 //returns coord
::smns::defs::real map_to_screen_relative_height(int pix_coord, Window* win);

  //returns coords
::smns::defs::real map_to_screen_relative_width(int pix_coord, Window* win);

//crop to [0, height] in pixels
int crop_to_screen_height(int pix_coord, Window* win);

//crop to [0, width] in pixels
int crop_to_screen_width(int pix_coord, Window* win);

//crop to [0.0, 1.0]
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
num_t crop(num_t pix_coord) {
  num_t res = std::min((std::max(pix_coord, 0_r)), 1.0_r);
  return res;
}

//crop to [0.0, 1.0]
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
vec2<num_t> crop(vec2<num_t> rel_coord) {
  vec2<num_t> res = { crop(rel_coord.x), crop(rel_coord.y) };
  return res;
}

void check(bool success);

struct audio_stream_data {
  SDL_AudioStream* _p_stream = nullptr;
  uint64_t* _p_streams_playing = nullptr;
  uint64_t* _p_streams_existing = nullptr;
  Audio* _p_audio = nullptr;

  std::atomic<size_t> _last_byte_it = 0;
  bool _is_playing = false;
  size_t _iter = 0;



  audio_stream_data(SDL_AudioStream* p_stream, uint64_t* p_streams_playing,
                    uint64_t* p_streams_existing, Audio* p_audio);
  audio_stream_data() = delete;

  audio_stream_data(const audio_stream_data& other) = delete;
  audio_stream_data& operator=(const audio_stream_data& other) = delete;

  audio_stream_data(audio_stream_data&& other) = delete;
  audio_stream_data& operator=(audio_stream_data&& other) = delete;

  ~audio_stream_data();
};

}  // namespace sdl_hlprs