#include <chrono>
#include <iostream>
#include <random>
#include <functional>
#include <complex>
#include "math/defs.h"
#include "obj/heart.h"
#include "app/app.h"
#include "math/analytics.h"


using namespace smns::defs::literals;
using namespace std::chrono_literals;

std::random_device rd{};
std::mt19937_64 RND(rd());

  const std::vector<std::string> adjectives = {
      "lucky",   "very lucky",   "incredibly lucky",
      "happy",   "the happiest", "wonderful",
      "amazing", "brave",        "adorable",
      "sweet",   "cute",         "lovely",
      "silly",   "strong",       "magical"};

  const std::vector<std::string> nouns = {
      "hubs",     "husbands",     "pups",       "boys",    "beans",
      "heros",    "sweeathearts", "honey buns", "buns",    "babes",
      "sweeties", "kings",        "angels",     "wizzards"};

  const std::vector<std::string> emodji = {
      ">w<", "owo", "OwO", "uwu", "UwU", "<3", ":3", ";3", "x3", ":p", ">///<"};

  const std::vector<std::string> wishes = {"nyom", "mwah",    "slurp", "nuzzle",
                                           "hugs", "cuddles", "xoxo"};


  const std::string& take_rnd_el(const std::vector<std::string>& str) {
    return str[RND() % str.size()];
  }

// returns random title
  std::string rnd_title() {
    std::string title;
    title += take_rnd_el(emodji);
    title += " ";
    title += take_rnd_el(adjectives);
    title += " ";
    title += take_rnd_el(nouns);
    title += " ";
    title += take_rnd_el(emodji);
    return title;
  }

int main() {
  using clock = std::chrono::high_resolution_clock;

  app m_app(24, "res/wav/");

  auto WHOA_IT = m_app.add_audio("LEGALIZENUCLEAR.wav", 0.3_r);

  auto win1 = m_app.add_window(rnd_title(), 1000, 1000);
  //auto win2 = m_app.add_window(rnd_title(), 1000, 1000);
  //auto win3 = m_app.add_window(rnd_title(), 1000, 1000);



  heart<HEART_TYPES::PARABOLA> m_heart(0.05, 0.06, 3.9, 25, 1, 1);
  m_heart.pos = {0.5, 0.5};
  m_app.add_obj(&m_heart);



  m_app.loop();
}