#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

struct audio;

template <typename... msg_ts>
void out(msg_ts&&... args) {
  (std::cout << ... << args) << '\n';
}

namespace smns {
namespace sdl_hlprs {

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
}  // namespace smns