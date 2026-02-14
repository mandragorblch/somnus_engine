#include "sdl_hlprs.h"

#include "app/audio.h"

void smns::sdl_hlprs::check(bool success) {
  if (!success) {
    std::cerr << SDL_GetError();
  }
}

smns::sdl_hlprs::audio_stream_data::audio_stream_data(
    SDL_AudioStream* p_stream, uint64_t* p_streams_playing,
    uint64_t* p_streams_existing, audio* p_audio)
    : _p_stream(p_stream),
      _p_streams_playing(p_streams_playing),
      _p_streams_existing(p_streams_existing),
      _p_audio(p_audio) {}

smns::sdl_hlprs::audio_stream_data::~audio_stream_data() {
  check(SDL_PauseAudioStreamDevice(_p_stream));
  SDL_DestroyAudioStream(_p_stream);
}
