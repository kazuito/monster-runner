#include "game.h"

Game::Game(int fps) {
  frameCount = 0;
  defaultFps = fps;
  currentFps = 0;
  fpsCounter = 0;
  fpsLastUpdatedTime = 0;
}

void Game::calc() {
  frameCount++;
  Uint64 now = SDL_GetTicks64();
  if ((now - fpsLastUpdatedTime) >= 1000) {
    currentFps = fpsCounter;
    fpsCounter = 0;
    fpsLastUpdatedTime = now;
  };
  fpsCounter++;
}