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
#include "core/audio.h"
#include "smns/types/average.hpp"
#include "../objects/base_objects/Renderer.h"
#include "../core/Scene.h"

class Window;
class Object;

class App {
 public:
  using real = smns::defs::real;
  using clock = std::chrono::high_resolution_clock;

  SDL_Event _event{};
  std::map<SDL_WindowID, class Window*> _windows{};
  std::queue<SDL_WindowID> _windows_to_close{};

  std::map<std::string, Audio*> _audios{};
  std::list<Audio*> _active_audios{};
  std::string _audio_files_path;
  real _master_volume{};

	smns::core::Scene scene;

  real _trgt_FPS{};
  std::chrono::microseconds _trgt_frame_time{};
  clock::time_point _FPStimer{};
  //to print FPS every second
  clock::time_point _FPS_latch_1_second{};
  //user defined time for averaging FPS
  //15 seconds by default
  clock::time_point _FPS_averaging_latch{};
  std::chrono::seconds _FPS_averaging_time{};
  uint32_t FPS_counter{};
  smns::types::math::average_t<> FPS_average{};

	real dt{};

  int _displays_count{};
  SDL_DisplayID* _displays = nullptr;

  //TODO decide to keep or remove
  //vec2<int> _resolution{};
  //real _aspect_ratio{};

  bool running = true;

  App(real target_FPS, const std::string& audio_files_path,
      std::chrono::seconds _FPS_averaging_time = std::chrono::seconds(15),
      real master_volume = real{1.0});
  ~App();

  auto add_window(const std::string& title, int width, int height)
      -> decltype(_windows)::iterator;

  void draw_frame();


  auto add_audio(const std::string& filename, real volume) -> decltype(_audios)::iterator;
  //put buffer to Audio stream
  void play_audio(decltype(_audios)::iterator audio_pair_it);
  //put buffer to Audio stream
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
