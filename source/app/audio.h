#pragma once
#include <SDL3/SDL.h>

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <list>

#include "smns/defs.h"

namespace smns::sdl_hlprs {
struct audio_stream_data;
}

constexpr int MAX_AUDIOSTREAMS_AMOUNT = 256;

class audio {
 public:
  using real = smns::defs::real;
  using audio_stream_data = smns::sdl_hlprs::audio_stream_data;
  using stream_data_cntnr_t = std::vector<std::unique_ptr<audio_stream_data>>;
  SDL_AudioDeviceID _devID{};

  // TODO delete
  // std::vector<SDL_AudioStream*> _streams{};

  stream_data_cntnr_t _stream_datas_active{};
  stream_data_cntnr_t _stream_datas_free_pool{};

  std::array<size_t, MAX_AUDIOSTREAMS_AMOUNT> finished_ring{};
  std::atomic<size_t> write_index = 0;
  std::atomic<size_t> read_index  = 0;
  std::vector<size_t> to_remove{};

  // dont touch, original audio file stored here
  Uint8* _audioBuf = nullptr;

  Uint32 _audioLen = 0;
  SDL_AudioSpec _spec{};
  // TODO delete
  // SDL_AudioStream* _last_stream_used = nullptr;

  uint64_t _streams_playing{};
  uint64_t _streams_existing{};
  //TODO change current volume on _volume change
  real _volume{};
  real* _p_master_volume = nullptr;
  //real _cur_volume{};

  audio(const std::string& filename, real* p_master_volume, real volume = real{1.0},
        SDL_AudioDeviceID deviceID = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);

  ~audio();

  //void process_audio_buff();

  // does NOT continue paused streams
  bool play();

  void tick();

  // remember to continue!
  void pause();

  void continue_();

  bool is_playing();

  audio_stream_data* create_stream();

  audio_stream_data* acquire_stream();

  void move_stream_data(stream_data_cntnr_t& src,
                        stream_data_cntnr_t& dst);

  void clear_streams();

  void clean_up();
};