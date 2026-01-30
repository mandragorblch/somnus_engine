#include "audio.h"

audio::audio(const std::string& filename, real* p_master_volume, real volume,
             SDL_AudioDeviceID deviceID)
    : _devID(deviceID), _p_master_volume(p_master_volume), _volume(volume) {
  if (!SDL_LoadWAV(filename.data(), &_spec, &_originalAudioBuf, &_audioLen)) {
    std::cout << SDL_GetError();
  }

  _processedAudioBuf = new Uint8[_audioLen];

  process_audio_buff();

  _devID = SDL_OpenAudioDevice(_devID, &_spec);
  if (_devID == 0) {
    std::cout << SDL_GetError();
  }
}

bool audio::play() {
  // needed to decide is its already playing to add/not add to active audios
  bool res = is_playing();
  SDL_AudioStream* free_stream = find_free_stream();

  if (free_stream ==
      nullptr) {  // TODO smth related with creating second audiostream
    free_stream = create_stream();
  }

  if (real master_volume = *_p_master_volume;
      _processedAudioBuf[0] != master_volume * _volume * _originalAudioBuf[0]) {
    process_audio_buff();
  }

  if (SDL_PutAudioStreamData(free_stream, _processedAudioBuf, _audioLen) < 0) {
    std::cerr << "Failed to push audio: " << SDL_GetError() << std::endl;
  }

  SDL_ResumeAudioStreamDevice(free_stream);

  _last_stream_used = free_stream;

  return res;
}

void audio::pause() {
  for (auto* p_stream : _streams) {
    SDL_PauseAudioStreamDevice(p_stream);
  }
};

bool audio::is_playing() {
  return (_last_stream_used != nullptr &&
          SDL_GetAudioStreamAvailable(_last_stream_used));
}

SDL_AudioStream* audio::create_stream() {
  SDL_AudioStream* s = SDL_OpenAudioDeviceStream(_devID, &_spec, NULL, NULL);
  _streams.push_back(s);

  if (_streams.back() == NULL) {
    std::cout << SDL_GetError();
  }

  SDL_BindAudioStream(_devID, _streams.back());

  return _streams.back();
}

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

void audio::clear_streams() {
  for (auto* s : _streams) {
    clear_stream(s);
  }
  _last_stream_used = nullptr;
  _streams.clear();
}

void audio::clear_stream(SDL_AudioStream* stream) {
  SDL_PauseAudioStreamDevice(stream);
  SDL_DestroyAudioStream(stream);
}

void audio::clean_up() {
  if (_streams.empty()) return;
  for (auto s_it = _streams.begin() + 1; s_it != _streams.end(); ++s_it) {
    clear_stream(*s_it);
  }
  _last_stream_used = nullptr;
  _streams.resize(1);
  SDL_PauseAudioStreamDevice(_streams.front());
}

audio::~audio() {
  clear_streams();
  SDL_CloseAudioDevice(_devID);
  SDL_free(_originalAudioBuf);
  delete[] _processedAudioBuf;
}

void audio::process_audio_buff() {
  real final_volume = (*_p_master_volume) * _volume;

  int bytesPerSample = SDL_AUDIO_BYTESIZE(_spec.format);
  int totalSamples = _audioLen / bytesPerSample;

  switch (_spec.format) {
    case SDL_AUDIO_F32: {
      float* oBuff = reinterpret_cast<float*>(_originalAudioBuf);
      float* pBuff = reinterpret_cast<float*>(_processedAudioBuf);
      for (int i = 0; i < totalSamples; ++i) pBuff[i] = oBuff[i] * final_volume;
      break;
    }
    case SDL_AUDIO_S16: {
      Sint16* oBuff = reinterpret_cast<Sint16*>(_originalAudioBuf);
      Sint16* pBuff = reinterpret_cast<Sint16*>(_processedAudioBuf);
      for (int i = 0; i < totalSamples; ++i) {
        int32_t v = static_cast<int32_t>(oBuff[i]) * final_volume;
        pBuff[i] = static_cast<Sint16>(SDL_clamp(v, -32768, 32767));
      }
      break;
    }
    case SDL_AUDIO_S32: {
      Sint32* oBuff = reinterpret_cast<Sint32*>(_originalAudioBuf);
      Sint32* pBuff = reinterpret_cast<Sint32*>(_processedAudioBuf);
      for (int i = 0; i < totalSamples; ++i) {
        int64_t v = static_cast<int64_t>(oBuff[i]) * final_volume;
        pBuff[i] = static_cast<Sint32>(SDL_clamp(v, -32768, 32767));
      }
      break;
    }
    case SDL_AUDIO_U8: {
      for (int i = 0; i < totalSamples; ++i) {
        int32_t v = static_cast<int32_t>(_originalAudioBuf[i] - 128) * final_volume + 128;
        _processedAudioBuf[i] = static_cast<Uint8>(SDL_clamp(v, 0, 255));
      }
      break;
    }
    default:
      // unsupported format
      break;
  }
}
