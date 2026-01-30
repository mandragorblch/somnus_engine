#include <chrono>
#include <iostream>
#include <limits>

#include "math/defs.h"
#include "obj/heart.h"
#include "app/app.h"

using namespace smns::defs;
using namespace std::chrono_literals;

int main() {
  using clock = std::chrono::high_resolution_clock;
  app m_app(60, "res/wav/");



  auto WHOA_IT = m_app.add_audio("LEGALIZENUCLEAR.wav", 0.1_r);
  //TODO randomizer of window titles
  auto win1 = m_app.add_window(">w< lucky hubs >w<", 1000, 1000);
  auto win2 = m_app.add_window(">w< very lucky hubs >w<", 1000, 1000);
  auto win3 = m_app.add_window(">w< incredibly lucky hubs >w<", 1000, 1000);



  m_app.loop();
}