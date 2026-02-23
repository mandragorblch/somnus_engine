#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <math/vec2.h>
#include <algorithm>
#include "app/window.h"

struct audio;

template <typename... msg_ts>
void out(msg_ts&&... args) {
  (std::cout << ... << args) << '\n';
}

namespace smns::sdl_hlprs {

  //returns coords in pixels
template<typename num_t = real>
  requires(std::is_arithmetic_v<num_t>)
int map_to_screen_width(num_t rel_coord, window* win) {
  int res = std::min(static_cast<int>(win->_width * rel_coord), win->_width - 1);
  return res;
}

 //returns coords in pixels
template<typename num_t = real>
  requires(std::is_arithmetic_v<num_t>)
int map_to_screen_height(num_t rel_coord, window* win) {
  int res = std::min(static_cast<int>(win->_height * (static_cast<num_t>(1) - rel_coord)),
                          win->_height - 1);
  return res;
}

  //returns coords in pixels
template<typename num_t = real>
  requires(std::is_arithmetic_v<num_t>)
vec2<int> map_to_screen(const vec2<num_t>& rel_coords, window* win) {
  vec2<int> res = {map_to_screen_width(rel_coords.x, win), map_to_screen_height(rel_coords.y, win)};
  return res;
}

 //returns coord
real map_to_screen_relative_height(int pix_coord, window* win);

  //returns coords
real map_to_screen_relative_width(int pix_coord, window* win);

//crop to [0, height] in pixels
int crop_to_screen_height(int pix_coord, window* win);

//crop to [0, width] in pixels
int crop_to_screen_width(int pix_coord, window* win);

//crop to [0.0, 1.0]
template<typename num_t = real>
  requires(std::is_arithmetic_v<num_t>)
num_t crop_to_screen(num_t pix_coord) {
  num_t res = std::min((std::max(pix_coord, 0_r)), 1.0_r);
  return res;
}

//crop to [0.0, 1.0]
template<typename num_t = real>
  requires(std::is_arithmetic_v<num_t>)
vec2<num_t> crop_to_screen(vec2<num_t> rel_coord) {
  vec2<num_t> res = { crop_to_screen(rel_coord.x), crop_to_screen(rel_coord.y) };
  return res;
}

void check(bool success);

struct audio_stream_data {
  SDL_AudioStream* _p_stream = nullptr;
  uint64_t* _p_streams_playing = nullptr;
  uint64_t* _p_streams_existing = nullptr;
  audio* _p_audio = nullptr;

  std::atomic<size_t> _last_byte_it = 0;
  bool _is_playing = false;
  size_t _iter = 0;



  audio_stream_data(SDL_AudioStream* p_stream, uint64_t* p_streams_playing,
                    uint64_t* p_streams_existing, audio* p_audio);
  audio_stream_data() = delete;

  audio_stream_data(const audio_stream_data& other) = delete;
  audio_stream_data& operator=(const audio_stream_data& other) = delete;

  audio_stream_data(audio_stream_data&& other) = delete;
  audio_stream_data& operator=(audio_stream_data&& other) = delete;

  ~audio_stream_data();
};

}  // namespace sdl_hlprs