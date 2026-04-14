#include "App.h"
#include "helpers/sdl_hlprs.h"
#include <thread>
#include "core/window.h"
#include "objects/base_objects/Object.h"
#include "smns/defs.h"

using namespace std::chrono_literals;
using namespace smns::sdl_hlprs;
using namespace smns::defs;

App::App(real target_FPS, const std::string& audio_files_path, std::chrono::seconds FPS_averaging_time, real master_volume)
    : _trgt_FPS(target_FPS),
      _trgt_frame_time(static_cast<int64_t>(1ll / _trgt_FPS)),
      _audio_files_path(audio_files_path),
      _FPS_averaging_time(FPS_averaging_time),
      _master_volume(master_volume){
  // Initialize SDL3
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cout << SDL_GetError();
  }
  if (!SDL_Init(SDL_INIT_AUDIO)) {
    std::cout << SDL_GetError();
  }

  //TODO multidisplay
  _displays = SDL_GetDisplays(&_displays_count);
}

App::~App() {}

auto App::add_window(const std::string& title, int width, int height)
    -> decltype(_windows)::iterator {
  Window* new_win = new Window(this, title, width, height);
  SDL_WindowID win_id = SDL_GetWindowID(new_win->_window);
  auto [iter, is_inserted] = _windows.insert({win_id, new_win});
  return iter;
}

void App::draw_frame() {
  for (auto& curr_pair : _windows) {
    auto* curr_win = curr_pair.second;
    auto surf = curr_win->_surface;

    check(SDL_LockSurface(surf));
    SDL_FillSurfaceRect(surf, NULL, 0);

    scene.render(curr_win);

    SDL_UnlockSurface(surf);
    check(SDL_UpdateWindowSurface(curr_win->_window));
  }
}

auto App::add_audio(const std::string& filename, real volume)
    -> decltype(_audios)::iterator {
  _audio_files_path.append(filename);

  Audio* new_audio = new Audio(_audio_files_path, &_master_volume, volume);
  auto [iter, is_inserted] = _audios.insert({filename, new_audio});

  _audio_files_path.erase(_audio_files_path.end() - filename.size(),
                          _audio_files_path.end());
  return iter;
}

void App::play_audio(decltype(_audios)::iterator audio_pair_it) {
  Audio* current = audio_pair_it->second;
  if (!current->play()) {
    _active_audios.push_back(current);
  }
}

void App::play_audio(const std::string& filename) {
  Audio* current = _audios.at(filename);
  if (!current->play()) {
    _active_audios.push_back(current);
  }
}

//call all needed funtions
void App::tick() {
  _wait_next_frame();

  draw_frame();

  _audio_tick();

  _callback_tick();

  _cleanup_tick();
}

//wait till frame time given by FPS
void App::_wait_next_frame() {
  while (clock::now() - _FPStimer < _trgt_frame_time);

  //optimized waiting
  /*auto time_elapsed = clock::now() - _FPStimer;
  if (time_elapsed < _trgt_frame_time)
    std::this_thread::sleep_for(_trgt_frame_time - (clock::now() - _FPStimer));*/

  //TODO let user choose averaging time for FPS_average
  //fps print
  ++FPS_counter;
  if ((clock::now() - _FPS_latch_1_second) >= 1s) {
    //clear line before printing
    if ((clock::now() - _FPS_averaging_latch) >= _FPS_averaging_time) {
      FPS_average.reset();
      _FPS_averaging_latch = clock::now();
    }
    FPS_average += FPS_counter;
    std::cout << "\r\033[K" << FPS_average();
    _FPS_latch_1_second = clock::now();

    FPS_counter = 0;
    return;
  }

  //estimated fps based on frametime
  //std::cout << '\r' << (1s / (clock::now() - _FPStimer));

  _FPStimer = clock::now();
}

//Audio magic
//can be called every few ticks
void App::_audio_tick() {
    for (auto* curr : _active_audios) {
    curr->tick();
  }

  std::erase_if(_active_audios, [](Audio* curr) {
    if (!curr->is_playing()) {
      curr->clean_up();
      return true;
    }
    return false;
  });
}

// tackle callbacks
void App::_callback_tick() {
  while (SDL_PollEvent(&_event)) {
    switch (_event.type) {
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        _windows_to_close.push(_event.window.windowID);
        break;

      // TODO resizable windows
      case SDL_EVENT_WINDOW_RESIZED: {
        auto* win = _windows.at(_event.window.windowID);
        win->_width = _event.window.data1;
        win->_height = _event.window.data2;
      } break;

      case SDL_EVENT_QUIT:
        // TODO silly thing;
        std::cout << "\nu silly, dont close all windows :p\n";
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        // play_audio("LEGALIZENUCLEAR.wav");
        {
          auto* win = _windows.at(_event.window.windowID);
          scene.objects[0]->pos.x = map_to_screen_relative_width(
              static_cast<int>(_event.button.x), win);
          scene.objects[0]->pos.y = map_to_screen_relative_height_flip(
              static_cast<int>(_event.button.y), win);
        }
        break;

      case SDL_EVENT_MOUSE_MOTION:
        if (_event.motion.state & SDL_BUTTON_LMASK) {
          auto* win = _windows.at(_event.window.windowID);
          scene.objects[0]->pos.x = map_to_screen_relative_width(
              static_cast<int>(_event.button.x), win);
          scene.objects[0]->pos.y = map_to_screen_relative_height_flip(
              static_cast<int>(_event.button.y), win);
        }

        if (_event.motion.state & SDL_BUTTON_RMASK) {
          printf("Dragging with right mouse\n");
        }
        break;
    }
  }
}

//clean up everything that queued in corresponding containers
void App::_cleanup_tick() {
  while (!_windows_to_close.empty()) {
    auto curr_win = _windows.find(_windows_to_close.front());
    _windows_to_close.pop();
    if (curr_win != _windows.end()) {
      delete curr_win->second;
      _windows.erase(curr_win);
    }
  }
}

//initialize all variables before loop started
void App::initialize_loop() {
  _FPStimer = clock::now();
  _FPS_latch_1_second = clock::now();
  _FPS_averaging_latch = clock::now();
}

void App::loop() {
  initialize_loop();
  do {
    tick();

   } while (running);
}
