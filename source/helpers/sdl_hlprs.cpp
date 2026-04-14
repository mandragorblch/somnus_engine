#include "sdl_hlprs.h"

#include "core/audio.h"
#include "core/window.h"
#include "smns/defs.h"

using namespace smns::defs::literals;
using real = smns::defs::real;

namespace smns::sdl_hlprs {

int flip_y(int pix_coord, Window* win) {
  return (win->_height - 1) - pix_coord;
}

int flip_x(int pix_coord, Window* win) {
  return (win->_width - 1) - pix_coord;
}

real map_to_screen_relative_height_flip(int pix_coord, Window* win) {
  real res = pix_coord;
  res /= (win->_height - 1);
  res = 1_r - res;
  return res;
}

real map_to_screen_relative_height(int pix_coord, Window* win) {
  real res = pix_coord;
  res /= (win->_height - 1);
  return res;
}

real map_to_screen_relative_width(int pix_coord, Window* win) {
  real res = pix_coord;
  res /= (win->_width - 1);
  return res;
}

int crop_to_screen_height(int pix_coord, Window* win) {
  int res = std::min((std::max(pix_coord, 0)), win->_height - 1);
  return res;
}

int crop_to_screen_width(int pix_coord, Window* win) {
  int res = std::min((std::max(pix_coord, 0)), win->_width - 1);
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