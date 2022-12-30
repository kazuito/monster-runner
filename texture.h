#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>

#include <iostream>
#include <vector>

typedef struct cellRange {
  int startCol;
  int startRow;
  int endCol;
  int endRow;
};

typedef struct xy {
  float x;
  float y;
};

typedef struct nCell {
  int nCols;
  int nRows;
};

typedef struct t_Texture {
  nCell cell;
  char* imgPath;
  SDL_Texture* tex;
  SDL_Rect* srcRect;
  SDL_FRect* dstRect;
  //SDL_Rect* boundRect;
};

typedef struct t_Animation {
  int state;
  std::vector<int> frameIndexes;
  int nFrames;
  int interval;
  bool loop;
};

class Texture {
 public:
  SDL_FRect dstRect;
  char* blockId;
  xy pos;
  xy drawPos;
  xy initialPos;
  xy vel;
  xy flip;
  int state;
  int prevState;
  std::vector<t_Animation*> anmVec;
  float expansionRate;

  Texture();
  Texture(SDL_Renderer* rend, int* frameCount, char* blockId);

  float getW();
  float getH();

  void setTextureByFile(nCell cell, char* imgPath, float expansion);
  void setTexture(nCell cell, SDL_Texture* tex, float expansion);

  void setInitPos(xy initPos);

  void addAnm(int state, std::vector<int> frameIndexes, int interval,
              bool loop);

  void addVel();

  void drawAnm();

  void drawTex();

  void clean();

 private:
  int* globalFrameCount;
  int frameCounter;
  std::vector<xy> srcRectCoords;
  SDL_Rect srcRect;
  SDL_Renderer* rend;
  SDL_Texture* texture;
  int lastUpdatedFrameCount;
  int nCols;
  int nRows;

  t_Animation* getAnmByState(int state);

  void textureInit();
};