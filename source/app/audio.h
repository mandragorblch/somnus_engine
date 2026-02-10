#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include "math/defs.h"

using namespace smns::defs;

//TODO uncrihgify it
//my own helper
namespace smns {
namespace SDL_helpers {
  struct audio_stream_data {
    SDL_AudioStream* _p_stream = nullptr;
    uint64_t* _p_streams_playing = nullptr;
    uint64_t* _p_streams_existing = nullptr;
    std::atomic<size_t> _last_byte_it;
    bool _is_playing = false;



    audio_stream_data(SDL_AudioStream* p_stream, uint64_t* p_streams_playing,
                      uint64_t* p_streams_existing)
        : _p_stream(p_stream),
          _p_streams_playing(p_streams_playing),
          _p_streams_existing(p_streams_existing),
          _last_byte_it(0),
          _is_playing(false) {}
    //audio_stream_data() = delete;

    audio_stream_data(const audio_stream_data& other) = delete;
    audio_stream_data& operator=(const audio_stream_data& other) = delete;

    audio_stream_data(audio_stream_data&& other) = delete;
    audio_stream_data& operator=(audio_stream_data&& other) = delete;
    //~audio_stream_data() = default;
  };
}  // namespace smns
}

struct audio {
    using audio_stream_data = smns::SDL_helpers::audio_stream_data;
  SDL_AudioDeviceID _devID;
  //TODO delete
  std::vector<SDL_AudioStream*> _streams{};

  std::vector<std::unique_ptr<audio_stream_data>> _stream_datas{};
  //dont touch, original audio file stored here
  Uint8* _originalAudioBuf = nullptr;
  //apply volume
  Uint8* _processedAudioBuf = nullptr;

  Uint32 _audioLen = 0;
  SDL_AudioSpec _spec;
  //TODO delete
  SDL_AudioStream* _last_stream_used = nullptr;

  uint64_t _streams_playing{};
  uint64_t _streams_existing{};
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