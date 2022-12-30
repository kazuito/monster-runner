#include "stage.h"

// Private

// Public
Stage::Stage(SDL_Renderer* rend, int* globalFrameCount, int screenHeight,
             char* filePath, std::vector<BlockData> blockDataVec) {
  camera = {0, 0};
  stageCol = 0;
  stageRow = 1;

  FILE* fp = fopen(filePath, "r");
  if (fp == nullptr) {
    printf("error opening stage map file\n");
    return;
  }

  char* bf = new char[3];
  bool scanLoop = true;
  bool isFirstLine = true;

  textureVec.resize(32);

  // Create textures
  for (const BlockData bd : blockDataVec) {
    if (bd.filePath == NULL) {
      textureVec[bd.id] = nullptr;
      continue;
    }
    SDL_Surface* tmp_sur = IMG_Load((const char*)bd.filePath);
    SDL_Texture* tmp_tex = SDL_CreateTextureFromSurface(rend, tmp_sur);
    textureVec[bd.id] = tmp_tex;
    SDL_FreeSurface(tmp_sur);
  }

  // Get the number of cols and rows of the stage
  while (scanLoop) {
    if (fscanf(fp, "%1[\n]", bf) == 1) {
      stageRow++;
      isFirstLine = false;
    } else if (fscanf(fp, " %2[^\n]", bf) == 1) {
      if (isFirstLine) stageCol++;
    } else {
      scanLoop = false;
    }
  }

  FILE* fp2 = fopen(filePath, "r");

  blocks.resize(stageRow);

  for (int i = 0; i < stageRow; i++) {
    blocks[i].resize(stageCol);

    for (int j = 0; j < stageCol; j++) {
      char* bf2 = new char[3];

      // Read AsciiId
      int nSuccess = fscanf(fp2, " %2s", bf2);
      if (nSuccess < 1) {
        printf("error loading stage map: %s\n", SDL_GetError());
        return;
      }

      // If empty block
      if (strcmp(bf2, "..") == 0) {
        blocks[i][j] = nullptr;
        continue;
      }

      BlockData curBlockData;

      // Get a block data by the AsciiId
      for (BlockData bd : blockDataVec) {
        if (strcmp(bd.asciiId, bf2) == 0) {
          curBlockData = bd;
          break;
        }
      }

      // Create new object from the Texture class
      Texture* tt = new Texture(rend, globalFrameCount, bf2);

      tt->setTexture(curBlockData.ncell, textureVec[curBlockData.id], 0.25);

      // Set animations
      for (AnmArgs aa : curBlockData.anmDataVec) {
        tt->addAnm(aa.stateId, aa.frameIndexes, aa.interval, aa.loop);
      }

      // Set position
      tt->setInitPos({float(j * tt->getW()),
                      float(screenHeight - (stageRow - i) * tt->getH())});

      blocks[i][j] = tt;
    }
  }
  printf("col:%d row:%d\n", stageCol, stageRow);
  printf("stage loaded successfully\n");
}

void Stage::draw(xy camera) {
  for (int i = 0; i < stageRow; i++) {
    for (int j = 0; j < stageCol; j++) {
      Texture* block = blocks[i][j];
      if (block == nullptr) {
        continue;
      }

      block->drawPos.x = block->initialPos.x - camera.x;
      block->drawPos.y = block->initialPos.y - camera.y;

      if (block->anmVec.size() > 0) {
        block->drawAnm();
      } else {
        block->drawTex();
      }
    }
  }
}

void Stage::clear() {
  for (int i = 0; i < stageRow; i++)
    for (int j = 0; j < stageCol; j++) {
      delete blocks[i][j];
    }
}