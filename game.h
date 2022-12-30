#pragma once
#include <SDL2/SDL.h>

#include <chrono>
#include <ctime>

class Game {
 public:
  int frameCount;
  float defaultFps;
  float currentFps;

  Game(int fps);
  void calc();

 private:
  Uint64 fpsLastUpdatedTime;
  int fpsCounter;
};