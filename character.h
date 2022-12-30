#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <vector>

#include "texture.h"

class Character : public Texture {
 public:
  float speed;
  float jumpPower;
  bool onGround;
  bool onWall;
  Character(SDL_Renderer* rend, int* frameCount, char* blockId);
};