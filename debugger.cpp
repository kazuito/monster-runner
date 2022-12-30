#include "debugger.h"

#include <stdio.h>

Debugger::Debugger(SDL_Renderer* rend) : rend(rend) {}

void Debugger::setFont(const char* filePath, int fontSize) {
  font = TTF_OpenFont(filePath, fontSize);
  if (font == nullptr) printf("error loading font in debugger\n");
}

void Debugger::drawText(int col, int row, const char* format, ...) {
  va_list args;
  char text[256] = "";

  va_start(args, format);
  vsprintf_s(text, format, args);
  va_end(args);

  SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, {0, 0, 0});
  SDL_Texture* textTexture =
      SDL_CreateTextureFromSurface(rend, textSurface);
  int textW, textH;
  SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
  int textX = 16, textY = 16 + row * textH;

  SDL_Rect rect = {textX, textY, textW, textH};
  SDL_RenderCopy(rend, textTexture, NULL, &rect);

  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

void Debugger::closeFont() { TTF_CloseFont(font); }
