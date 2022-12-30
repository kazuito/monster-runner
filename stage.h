#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

#include <vector>

#include "character.h"
#include "texture.h"

struct AnmArgs {
  int stateId;
  std::vector<int> frameIndexes;
  int interval;
  bool loop;
};

struct BlockData {
  int id;
  const char* asciiId;
  nCell ncell;
  const char* filePath;
  std::vector<AnmArgs> anmDataVec;
};

struct Block {
  int id;
  Texture tex;
};

class Stage {
 private:
 public:
  xy camera;
  std::vector<SDL_Texture*> textureVec;
  int stageCol;
  int stageRow;
  std::vector<std::vector<Texture*>> blocks;

  Stage(SDL_Renderer* rend, int* globalFrameCount, int screenHeight,
        char* filePath, std::vector<BlockData> blockDataVec);

  void draw(xy camera);
  void clear();
};
