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
int map_to_window_width(num_t coord, int window_width) {
  int res = static_cast<int>(std::floor((window_width - 1) * coord));
  return res;
}

 //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
int map_to_window_height(num_t coord, int window_height) {
  int res = static_cast<int>(std::floor((window_height - 1) * coord));
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
int flip_y(int pix_coord, int window_height);

//flip the coord
 //returns coords in pixels
int flip_x(int pix_coord, int window_width);

 //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
int map_to_window_height_flip(num_t rel_coord, int window_height) {
  int res = static_cast<int>((window_height - 1) * (1_r - rel_coord));
  return res;
}

  //returns coords in pixels with y flipped (it renders upside down)
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
vec2<int> map_to_window(const vec2<num_t>& rel_coords, int window_width, int window_height) {
  vec2<int> res = {map_to_window_width(rel_coords.x, window_width), map_to_window_height(rel_coords.y, window_height)};
  return res;
}

 //returns coords in pixels
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
vec2<int> map_to_window_size(const vec2<num_t>& rel_coords, int window_width, int window_height) {
  vec2<int> res = {map_to_window_width(rel_coords.x, window_width), map_to_window_height(rel_coords.y, window_height)};
  return res;
}

 //returns coord
::smns::defs::real map_to_window_relative_height_flip(int pix_coord, int window_height);

 //returns coord
::smns::defs::real map_to_window_relative_height(int pix_coord, int window_height);

  //returns coords
::smns::defs::real map_to_window_relative_width(int pix_coord, int window_width);

//crop to [0, height] in pixels
int crop_to_window_height(int pix_coord, int window_width);

//crop to [0, width] in pixels
int crop_to_window_width(int pix_coord, int window_width);

//crop to [0.0, 1.0]
template<typename num_t = ::smns::defs::real>
  requires(std::is_arithmetic_v<num_t>)
num_t crop(num_t pix_coord) {
  num_t res = std::clamp(pix_coord, 0_r, 1_r);
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