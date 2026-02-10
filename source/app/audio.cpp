#include "audio.h"
#include <assert.h>

audio::audio(const std::string& filename, real* p_master_volume, real volume,
             SDL_AudioDeviceID deviceID)
    : _devID(deviceID), _p_master_volume(p_master_volume), _volume(volume) {
  assert(SDL_LoadWAV(filename.data(), &_spec, &_originalAudioBuf, &_audioLen) && SDL_GetError());

  _processedAudioBuf = new Uint8[_audioLen];

  process_audio_buff();

  _devID = SDL_OpenAudioDevice(_devID, &_spec);
  assert(_devID && SDL_GetError());
}

//TODO rewrite
bool audio::play() {
  // needed to decide is its already playing to add/not add to active audios
  bool res = is_playing();
  SDL_AudioStream* free_stream = find_free_stream();

  if (free_stream ==
      nullptr) {
    free_stream = create_stream();
  }

  if (real master_volume = *_p_master_volume;
      _cur_volume != master_volume * _volume) {
    _cur_volume = master_volume * _volume;
    process_audio_buff();
  }

  assert(SDL_PutAudioStreamData(free_stream, _processedAudioBuf, _audioLen) && SDL_GetError());

  assert(SDL_ResumeAudioStreamDevice(free_stream) && SDL_GetError());

  _last_stream_used = free_stream;

  return res;
}

void audio::pause() {
  for (auto* p_stream : _streams) {
    SDL_PauseAudioStreamDevice(p_stream);
  }
};

//TODO rewrite to callbacks
bool audio::is_playing() {
  return (_last_stream_used != nullptr &&
          SDL_GetAudioStreamAvailable(_last_stream_used));
}

//TODO rewrite
SDL_AudioStream* audio::create_stream() {
  SDL_AudioStream* p_stream = SDL_OpenAudioDeviceStream(_devID, &_spec, NULL, NULL);
  assert(p_stream && SDL_GetError());

  bool success = SDL_BindAudioStream(_devID, p_stream);
  assert(success && SDL_GetError());

    //_streams.push_back(s);
  //audio_stream_data s_data(p_stream, &_streams_playing, &_streams_existing);
  _stream_datas.emplace_back(p_stream, &_streams_playing, &_streams_existing);

  return p_stream;
}

//TODO add queue?
SDL_AudioStream* audio::find_free_stream() {
  size_t free_index = 0uz;

  while (free_index < _streams.size() &&
         SDL_GetAudioStreamAvailable(_streams[free_index]) != 0) {
    ++free_index;
  }

  if (free_index == _streams.size()) {
    return nullptr;
  }

  return _streams[free_index];
}

//TODO rewrite
void audio::clear_streams() {
  for (auto* s : _streams) {
    clear_stream(s);
  }
  _last_stream_used = nullptr;
  _streams.clear();
}

//TODO rewrite
void audio::clear_stream(SDL_AudioStream* stream) {
  SDL_PauseAudioStreamDevice(stream);
  SDL_DestroyAudioStream(stream);
}

//TODO rewrite
void audio::clean_up() {
  if (_streams.empty()) return;
  for (auto s_it = _streams.begin() + 1; s_it != _streams.end(); ++s_it) {
    clear_stream(*s_it);
  }
  _last_stream_used = nullptr;
  _streams.resize(1);
  SDL_PauseAudioStreamDevice(_streams.front());
}

//void audio::audio_callback(void* userdata, SDL_AudioStream* stream,
//                           int additional_amount, int total_amount) {
//    //TODO link userdata to new struct that holding atomic iterator, maybe its good to make counter of playing streams
//  assert(SDL_PutAudioStreamData(stream, buffer.data(),
//                                buffer.size() * sizeof(int16_t)) &&
//         SDL_GetError());
//}

audio::~audio() {
  clear_streams();
  SDL_CloseAudioDevice(_devID);
  SDL_free(_originalAudioBuf);
  delete[] _processedAudioBuf;
}

void audio::process_audio_buff() {

  int bytesPerSample = SDL_AUDIO_BYTESIZE(_spec.format);
  int totalSamples = _audioLen / bytesPerSample;

  switch (_spec.format) {
    case SDL_AUDIO_F32: {
      float* oBuff = reinterpret_cast<float*>(_originalAudioBuf);
      float* pBuff = reinterpret_cast<float*>(_processedAudioBuf);
      for (int i = 0; i < totalSamples; ++i)
        pBuff[i] = static_cast<float>(oBuff[i] * _cur_volume);
      break;
    }
    case SDL_AUDIO_S16: {
      Sint16* oBuff = reinterpret_cast<Sint16*>(_originalAudioBuf);
      Sint16* pBuff = reinterpret_cast<Sint16*>(_processedAudioBuf);
      for (int i = 0; i < totalSamples; ++i) {
        int32_t v = static_cast<int32_t>(oBuff[i] * _cur_volume);
        pBuff[i] = static_cast<Sint16>(SDL_clamp(v, -32768, 32767));
      }
      break;
    }
    case SDL_AUDIO_S32: {
      Sint32* oBuff = reinterpret_cast<Sint32*>(_originalAudioBuf);
      Sint32* pBuff = reinterpret_cast<Sint32*>(_processedAudioBuf);
      for (int i = 0; i < totalSamples; ++i) {
        int64_t v = static_cast<int64_t>(oBuff[i] * _cur_volume);
        pBuff[i] = static_cast<Sint32>(SDL_clamp(v, -32768, 32767));
      }
      break;
    }
    case SDL_AUDIO_U8: {
      for (int i = 0; i < totalSamples; ++i) {
        int32_t v = static_cast<int32_t>((_originalAudioBuf[i] - 128) * _cur_volume + 128);
        _processedAudioBuf[i] = static_cast<Uint8>(SDL_clamp(v, 0, 255));
      }
      break;
    }
    default:
      // unsupported format
      break;
  }
}

//smns::SDL_helpers::audio_stream_data::audio_stream_data(
//    SDL_AudioStream* p_stream, uint64_t* p_streams_playing,
//    uint64_t* p_streams_existing)
//    : _p_stream(p_stream),
//      _p_streams_playing(p_streams_playing),
//      _p_streams_existing(p_streams_existing),
//      _last_byte_it(0),
//      _is_playing(false) {}
