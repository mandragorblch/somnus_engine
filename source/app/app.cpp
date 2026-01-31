#include "app.h"

using namespace std::chrono_literals;

app::app(float target_FPS, const std::string& audio_files_path, real master_volume)
    : _trgt_FPS(target_FPS),
      _trgt_frame_time(1s / _trgt_FPS),
      _FPStimer(clock::now()),
      _audio_files_path(audio_files_path),
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

app::~app() {}

auto app::add_window(const std::string& title, int width, int height)
    -> decltype(_windows)::iterator {
  window* new_win = new window(this, title, width, height);
  SDL_WindowID win_id = SDL_GetWindowID(new_win->_window);
  auto [iter, is_inserted] = _windows.insert({win_id, new_win});
  return iter;
}

void app::draw_frame() {
  for (auto& curr_pair : _windows) {
    window& curr_win = *(curr_pair.second);
    for (obj* curr_obj : objs) curr_obj->draw(curr_win._surface);
  }
}

auto app::add_audio(const std::string& filename, real volume)
    -> decltype(_audios)::iterator {
  _audio_files_path.append(filename);

  audio* new_audio = new audio(_audio_files_path, &_master_volume, volume);
  auto [iter, is_inserted] = _audios.insert({filename, new_audio});

  _audio_files_path.erase(_audio_files_path.end() - filename.size(),
                          _audio_files_path.end());
  return iter;
}

void app::play_audio(decltype(_audios)::iterator audio_pair_it) {
  audio* current = audio_pair_it->second;
  if (!current->play()) {
    _active_audios.push_back(current);
  }
}

void app::play_audio(const std::string& filename) {
  audio* current = _audios.at(filename);
  if (!current->play()) {
    _active_audios.push_back(current);
  }
}

//call all needed funtions
void app::tick() {
  _wait_next_frame();

  draw_frame();

  _audio_tick();

  _callback_tick();

  _cleanup_tick();
}

//wait till frame time given by FPS
void app::_wait_next_frame() {
  while (clock::now() - _FPStimer < _trgt_frame_time);
  std::cout << '\r' << (1s / (clock::now() - _FPStimer));
  _FPStimer = clock::now();
}

//audio magic
void app::_audio_tick() {
  std::erase_if(_active_audios, [](audio* curr) {
    if (!curr->is_playing()) {
      curr->clean_up();
      return true;
    }
    return false;
  });
}

//tackle callbacks
void app::_callback_tick() {
  while (SDL_PollEvent(&_event)) {
    switch (_event.type) {
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        _windows_to_close.push(_event.window.windowID);
        break;

      case SDL_EVENT_QUIT:
        // TODO silly thing;
        std::cout << "\nu silly, dont close all windows :p\n";
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        play_audio("LEGALIZENUCLEAR.wav");
        break;
    }
  }
}

//clean up everything that queued in corresponding containers
void app::_cleanup_tick() {
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
void app::initialize_loop() {
  auto _lastTime = clock::now();
}

void app::loop() {
  initialize_loop();
  do {
    tick();

   } while (running);
}
