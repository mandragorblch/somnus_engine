#include "audio.h"
#include "helpers/sdl_hlprs.h"
#include <algorithm>

#include <assert.h>

using namespace smns::sdl_hlprs;

void audio_callback(void* userdata, SDL_AudioStream* stream,
                           int additional_amount, int total_amount) {
  // TODO link userdata to new struct that holding atomic iterator, maybe
  // its good to make counter of playing streams
  static Sint16 buffer[4096];
  auto* p_stream_data = reinterpret_cast<audio_stream_data*>(userdata);
  auto* p_audio = p_stream_data->_p_audio;

  Sint16* audio_buff = reinterpret_cast<Sint16*>(p_audio->_audioBuf);
  size_t it = p_stream_data->_last_byte_it.load();
  int i = 0;
  //TODO change '/2' to actual size of data to know bytes
  int samples = additional_amount / 2;
  bool buffer_exhaust = false;
  smns::defs::real volume = p_audio->_volume * (*(p_audio->_p_master_volume));
  for (;
       (i < samples) &&
       (buffer_exhaust = ((it + i) * 2 >= p_audio->_audioLen), !buffer_exhaust);
       ++i) {
    int32_t v = static_cast<int32_t>(audio_buff[it + i] * volume);
    buffer[i] = static_cast<Sint16>(
        SDL_clamp(v, std::numeric_limits<Sint16>::min(), std::numeric_limits<Sint16>::max()));
  }

  p_stream_data->_last_byte_it.fetch_add(i);
  check(SDL_PutAudioStreamData(stream, buffer, i * 2));

  if (i != samples) {
    check(SDL_PauseAudioStreamDevice(stream));
    p_stream_data->_is_playing = false;
    --p_audio->_streams_playing;

    size_t w = p_audio->write_index.load(std::memory_order_relaxed);
    size_t next = (w + 1) % MAX_AUDIOSTREAMS_AMOUNT;

    if (next != p_audio->read_index.load(std::memory_order_acquire)) {
      p_audio->finished_ring[w] = p_stream_data->_iter;
      p_audio->write_index.store(next, std::memory_order_release);
    }
  }
}

audio::audio(const std::string& filename, real* p_master_volume, real volume,
             SDL_AudioDeviceID deviceID)
    : _devID(deviceID), _p_master_volume(p_master_volume), _volume(volume) {
  check(SDL_LoadWAV(filename.data(), &_spec, &_audioBuf, &_audioLen));

  _devID = SDL_OpenAudioDevice(_devID, &_spec);
  check(_devID);
}

bool audio::play() {
  // needed to decide is its already playing to add/not add to active audios
  bool res = is_playing();
  auto stream_data = acquire_stream();
  auto stream = stream_data->_p_stream;
   
  //check(SDL_PutAudioStreamData(stream, _processedAudioBuf, _audioLen));

  check(SDL_ResumeAudioStreamDevice(stream));

  stream_data->_is_playing = true;
  stream_data->_last_byte_it = 0;
  ++_streams_playing;

  return res;
}

//TODO reread revisit recycle
void audio::tick() {
  bool changed = read_index.load(std::memory_order_relaxed)
    != write_index.load(std::memory_order_acquire);

  while (read_index.load(std::memory_order_relaxed)
    != write_index.load(std::memory_order_acquire))
{
    size_t r = read_index.fetch_add(1, std::memory_order_relaxed);
    auto s = finished_ring[r];

    read_index.store((r + 1) % MAX_AUDIOSTREAMS_AMOUNT, std::memory_order_release);

    to_remove.push_back(s);
  }
  if (changed) {
    std::sort(to_remove.rbegin(), to_remove.rend());
    for (size_t idx : to_remove) {
      _stream_datas_free_pool.push_back(std::move(_stream_datas_active[idx]));

      if (idx != _stream_datas_active.size() - 1) {
        _stream_datas_active[idx] = std::move(_stream_datas_active.back());

        // update moved element index!
        _stream_datas_active[idx]->_iter = idx;
      }

      _stream_datas_active.pop_back();
    }
    to_remove.clear();
  }
}

void audio::pause() {
  for (const auto& stream_data : _stream_datas_active) {
    SDL_PauseAudioStreamDevice(stream_data.get()->_p_stream);
  }
}

void audio::continue_() {
  for (const auto& stream_data : _stream_datas_active) {
    SDL_ResumeAudioStreamDevice(stream_data.get()->_p_stream);
  }
}

bool audio::is_playing() { return _streams_playing > 0; };

audio_stream_data* audio::create_stream() {
  SDL_AudioStream* p_stream =
      SDL_OpenAudioDeviceStream(_devID, &_spec, NULL, NULL);
  check(p_stream);

  //_streams.push_back(s);
  // audio_stream_data s_data(p_stream, &_streams_playing, &_streams_existing);
  _stream_datas_active.emplace_back(std::make_unique<audio_stream_data>(
      p_stream, &_streams_playing, &_streams_existing, this));

  auto* stream_data = _stream_datas_active.back().get();
  stream_data->_iter = _stream_datas_active.size() - 1;
  //TODO make diff callbacks for diff data types SDL_AUDIO_S16
  SDL_AudioStreamCallback callback = &audio_callback;
  SDL_SetAudioStreamGetCallback(p_stream, callback,
                                stream_data);

  return stream_data;
}

audio_stream_data* audio::acquire_stream() {
  if (!_stream_datas_free_pool.empty()) {
    move_stream_data(_stream_datas_free_pool, _stream_datas_active);
    _stream_datas_active.back().get()->_iter = _stream_datas_active.size() - 1;
    return _stream_datas_active.back().get();
  } else {
    return create_stream();
  }
}

void audio::move_stream_data(stream_data_cntnr_t& src,
                             stream_data_cntnr_t& dst) {
  auto ptr = std::move(src.back());
  src.pop_back();
  dst.push_back(std::move(ptr));
}

void audio::clear_streams() {
  _stream_datas_active.clear();
  _stream_datas_free_pool.clear();
}

void audio::clean_up() {
  if (_stream_datas_free_pool.empty()) {
    if (!_stream_datas_active.empty()) {
      move_stream_data(_stream_datas_active, _stream_datas_free_pool);
    }
  } else {
    _stream_datas_free_pool.resize(1);
  }

  _stream_datas_active.clear();
}

audio::~audio() {
  SDL_CloseAudioDevice(_devID);
  SDL_free(_audioBuf);
}

//void audio::process_audio_buff() {
//  int bytesPerSample = SDL_AUDIO_BYTESIZE(_spec.format);
//  int totalSamples = _audioLen / bytesPerSample;
//
//  switch (_spec.format) {
//    case SDL_AUDIO_F32: {
//      float* oBuff = reinterpret_cast<float*>(_audioBuf);
//      float* pBuff = reinterpret_cast<float*>(_processedAudioBuf);
//      for (int i = 0; i < totalSamples; ++i)
//        pBuff[i] = static_cast<float>(oBuff[i] * _cur_volume);
//      break;
//    }
//    case SDL_AUDIO_S16: {
//      Sint16* oBuff = reinterpret_cast<Sint16*>(_audioBuf);
//      Sint16* pBuff = reinterpret_cast<Sint16*>(_processedAudioBuf);
//      for (int i = 0; i < totalSamples; ++i) {
//        int32_t v = static_cast<int32_t>(oBuff[i] * _cur_volume);
//        pBuff[i] = static_cast<Sint16>(SDL_clamp(v, -32768, 32767));
//      }
//      break;
//    }
//    case SDL_AUDIO_S32: {
//      Sint32* oBuff = reinterpret_cast<Sint32*>(_audioBuf);
//      Sint32* pBuff = reinterpret_cast<Sint32*>(_processedAudioBuf);
//      for (int i = 0; i < totalSamples; ++i) {
//        int64_t v = static_cast<int64_t>(oBuff[i] * _cur_volume);
//        pBuff[i] = static_cast<Sint32>(SDL_clamp(v, -32768, 32767));
//      }
//      break;
//    }
//    case SDL_AUDIO_U8: {
//      for (int i = 0; i < totalSamples; ++i) {
//        int32_t v = static_cast<int32_t>(
//            (_audioBuf[i] - 128) * _cur_volume + 128);
//        _processedAudioBuf[i] = static_cast<Uint8>(SDL_clamp(v, 0, 255));
//      }
//      break;
//    }
//    default:
//      // unsupported format
//      break;
//  }
//}

// smns::SDL_helpers::audio_stream_data::audio_stream_data(
//     SDL_AudioStream* p_stream, uint64_t* p_streams_playing,
//     uint64_t* p_streams_existing)
//     : _p_stream(p_stream),
//       _p_streams_playing(p_streams_playing),
//       _p_streams_existing(p_streams_existing),
//       _last_byte_it(0),
//       _is_playing(false) {}
