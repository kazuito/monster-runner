#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Debugger {
 public:
  int fontSize;
  TTF_Font* font;
  SDL_Renderer* rend;

  Debugger(SDL_Renderer* rend);
  void setFont(const char* filePath, int fontSize);
  void drawText(int col, int row, const char* format, ...);
  void closeFont();
};
