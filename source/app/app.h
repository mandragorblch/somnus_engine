#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <functional>
#include <chrono>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <queue>
#include "app/window.h"
#include "app/audio.h"
#include "obj/obj.h"

struct app {
  using clock = std::chrono::high_resolution_clock;

  SDL_Event _event{};
  std::map<SDL_WindowID, struct window*> _windows{};
  std::queue<SDL_WindowID> _windows_to_close{};

  std::map<std::string, audio*> _audios{};
  std::list<audio*> _active_audios{};
  std::string _audio_files_path;
  real _master_volume{};

  std::vector<obj*> objs{};

  float _trgt_FPS{};
  std::chrono::duration<decltype(_trgt_FPS), std::micro> _trgt_frame_time{};
  clock::time_point _FPStimer{};

  int _displays_count{};
  SDL_DisplayID* _displays = nullptr;

  //TODO decide to keep or remove
  //vec2<int> _resolution{};
  //real _aspect_ratio{};

  public:
  bool running = true;

  app(float target_FPS, const std::string& audio_files_path, real _master_volume = 1.0_r);
  ~app();

  auto add_window(const std::string& title, int width, int height)
      -> decltype(_windows)::iterator;

  void draw_frame();


  auto add_audio(const std::string& filename, real volume) -> decltype(_audios)::iterator;
  //put buffer to audio stream
  void play_audio(decltype(_audios)::iterator audio_pair_it);
  //put buffer to audio stream
  void play_audio(const std::string& filename);


  //u should call it every main loop
  void tick();

  void _wait_next_frame();

  void _audio_tick();

  void _callback_tick();

  void _cleanup_tick();
  
  void initialize_loop();

  void loop();
};
