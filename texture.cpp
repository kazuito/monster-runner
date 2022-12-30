#include "texture.h"

// Public
Texture::Texture() {
  rend = nullptr;
  blockId = 0;
  globalFrameCount = nullptr;
  pos = {0, 0};
  initialPos = {0, 0};
  vel = {0, 0};
  lastUpdatedFrameCount = -1;
  srcRect = {0, 0, 0, 0};
  dstRect = {0, 0, 0, 0};
  state = 0;
  prevState = -1;
  nCols = 0;
  nRows = 0;
  frameCounter = 0;
  srcRectCoords = {};
  anmVec = {};
  texture = nullptr;
  flip = {false, false};
  expansionRate = 1;
}
Texture::Texture(SDL_Renderer* rend, int* frameCount, char* blockId)
    : rend(rend), blockId(blockId), globalFrameCount(frameCount) {
  pos = {0, 0};
  initialPos = {0, 0};
  vel = {0, 0};
  lastUpdatedFrameCount = -1;
  srcRect = {0, 0, 0, 0};
  dstRect = {0, 0, 0, 0};
  state = 0;
  prevState = -1;
  nCols = 0;
  nRows = 0;
  frameCounter = 0;
  srcRectCoords = {};
  anmVec = {};
  texture = nullptr;
  flip = {false, false};
  expansionRate = 1;
};

float Texture::getH() { return dstRect.h ; }

float Texture::getW() { return dstRect.w ; }

void Texture::setTextureByFile(nCell cell, char* imgPath, float expansion) {
  if (imgPath == NULL) return;

  SDL_Surface* surface = IMG_Load(imgPath);
  texture = SDL_CreateTextureFromSurface(rend, surface);

  nCols = cell.nCols;
  nRows = cell.nRows;

  expansionRate = expansion;
  textureInit();

  SDL_FreeSurface(surface);
}

void Texture::setTexture(nCell cell, SDL_Texture* tex, float expansion) {
  if (tex == nullptr) return;

  expansionRate = expansion;
  texture = tex;

  nCols = cell.nCols;
  nRows = cell.nRows;

  textureInit();
}

void Texture::setInitPos(xy initPos) {
  initialPos = initPos;
  pos = initPos;
}

void Texture::addAnm(int state, std::vector<int> frameIndexes, int interval,
                     bool loop) {
  std::vector<xy> srcRectCoords;
  t_Animation* newAnm = new t_Animation;

  newAnm->state = state;
  newAnm->nFrames = frameIndexes.size();
  newAnm->frameIndexes = frameIndexes;
  newAnm->interval = interval;
  newAnm->loop = loop;

  anmVec.push_back(newAnm);
}

void Texture::addVel() {
  pos.x += vel.x / 10;
  pos.y += vel.y / 10;
}

void Texture::drawAnm() {
  t_Animation* anm = getAnmByState(state);

  if (anm == nullptr) {
    printf(
        "error: Texture::drawAnm() was failed because anmVec[%d] is nullptr\n",
        state);
    return;
  }

  if (lastUpdatedFrameCount == -1) {
    lastUpdatedFrameCount = *globalFrameCount;
  }

  if (prevState != state) {
    frameCounter = 0;
    lastUpdatedFrameCount = *globalFrameCount;
  } else if ((*globalFrameCount - lastUpdatedFrameCount) % anm->interval == 0) {
    if (anm->loop) {
      frameCounter = (frameCounter + 1) % anm->nFrames;
      lastUpdatedFrameCount = *globalFrameCount;
    } else {
      if (frameCounter < anm->nFrames - 1) frameCounter++;
    }
  }

  int frameIndex = anm->frameIndexes[frameCounter];

  dstRect.x = drawPos.x;
  dstRect.y = drawPos.y;

  srcRect.x = srcRectCoords[frameIndex].x;
  srcRect.y = srcRectCoords[frameIndex].y;

  SDL_RendererFlip flipFlag = SDL_FLIP_NONE;
  if (flip.y) flipFlag = SDL_FLIP_VERTICAL;
  if (flip.x) flipFlag = SDL_FLIP_HORIZONTAL;

  SDL_RenderCopyExF(rend, texture, &srcRect, &dstRect, NULL, NULL, flipFlag);

  prevState = state;
}

void Texture::drawTex() {
  dstRect.x = drawPos.x;
  dstRect.y = drawPos.y;
  srcRect.x = 0;
  srcRect.y = 0;

  SDL_RenderCopyF(rend, texture, &srcRect, &dstRect);
}

void Texture::clean() { SDL_DestroyTexture(texture); }

// Private
t_Animation* Texture::getAnmByState(int state) {
  t_Animation* anm = new t_Animation;
  for (t_Animation* elem : anmVec) {
    if (elem->state == state) {
      anm = elem;
      return anm;
    }
  }
  return nullptr;
}
void Texture::textureInit() {
  int tmpW, tmpH;
  SDL_QueryTexture(texture, NULL, NULL, &tmpW, &tmpH);

  dstRect.w = tmpW;
  dstRect.h = tmpH;

  srcRect.w = dstRect.w / nCols;
  srcRect.h = dstRect.h / nRows;

  dstRect.w *= expansionRate;
  dstRect.h *= expansionRate;
  dstRect.w /= nCols;
  dstRect.h /= nRows;

  for (int i = 0; i < nRows; i++)
    for (int j = 0; j < nCols; j++) {
      srcRectCoords.push_back({(float)j * srcRect.w, (float)i * srcRect.h});
    }
}