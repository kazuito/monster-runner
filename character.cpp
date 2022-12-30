#include "character.h"

Character::Character(SDL_Renderer* rend, int* frameCount, char* blockId)
    : Texture(rend, frameCount, blockId), onGround(false), onWall(false) {
}