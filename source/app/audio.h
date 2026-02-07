#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include "math/defs.h"

using namespace smns::defs;

struct audio {
  SDL_AudioDeviceID _devID;
  std::vector<SDL_AudioStream*> _streams{};
  //dont touch, original audio file stored here
  Uint8* _originalAudioBuf = nullptr;
  //apply volume
  Uint8* _processedAudioBuf = nullptr;

  Uint32 _audioLen = 0;
  SDL_AudioSpec _spec;
  SDL_AudioStream* _last_stream_used = nullptr;
  real _volume{};
  real* _p_master_volume = nullptr;
  real _cur_volume{};

  audio(const std::string& filename, real* p_master_volume,
        real volume = 1.0_r,
        SDL_AudioDeviceID deviceID = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
  
  ~audio();

  

  void process_audio_buff();



  bool play();

  void pause();

  bool is_playing();

  SDL_AudioStream* create_stream();

  SDL_AudioStream* find_free_stream();

  void clear_streams();

  void clear_stream(SDL_AudioStream* stream);

  void clean_up();

  void audio_callback(void* userdata, SDL_AudioStream* stream,
                      int additional_amount, int total_amount);
};