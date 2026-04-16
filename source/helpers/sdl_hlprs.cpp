#include "sdl_hlprs.h"

#include "core/audio.h"
#include "core/window.h"
#include "smns/defs.h"

using namespace smns::defs::literals;
using real = smns::defs::real;

namespace smns::sdl_hlprs {

int flip_y(int pix_coord, int window_height) {
  return (window_height - 1) - pix_coord;
}

int flip_x(int pix_coord, int window_width) {
  return (window_width - 1) - pix_coord;
}

real map_to_window_relative_height_flip(int pix_coord, int window_height) {
  real res = map_to_window_relative_height(pix_coord, window_height);
  res = 1_r - res;
  return res;
}

real map_to_window_relative_height(int pix_coord, int window_height) {
  real res = pix_coord;
  res /= window_height - 1;
  return res;
}

real map_to_window_relative_width(int pix_coord,int window_width) {
  real res = pix_coord;
  res /= window_width - 1;
  return res;
}

int crop_to_window_height(int pix_coord, int window_height) {
  int res = std::clamp(pix_coord, 0, window_height - 1);
  return res;
}

int crop_to_window_width(int pix_coord, int window_width) {
  int res = std::clamp(pix_coord, 0, window_width - 1);
  return res;
}

void check(bool success) {
  if (!success) {
    std::cerr << SDL_GetError();
  }
}

audio_stream_data::audio_stream_data(SDL_AudioStream* p_stream,
                                     uint64_t* p_streams_playing,
                                     uint64_t* p_streams_existing,
                                     Audio* p_audio)
    : _p_stream(p_stream),
      _p_streams_playing(p_streams_playing),
      _p_streams_existing(p_streams_existing),
      _p_audio(p_audio) {}

audio_stream_data::~audio_stream_data() {
  check(SDL_PauseAudioStreamDevice(_p_stream));
  SDL_DestroyAudioStream(_p_stream);
}
}  // namespace smns::sdl_hlprs