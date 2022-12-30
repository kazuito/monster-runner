#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <stdio.h>

#include <chrono>

#include "character.h"
#include "debugger.h"
#include "game.h"
#include "stage.h"
#include "texture.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct CONTROLLER_STATE {
  Uint8 a;
  Uint8 b;
  Uint8 x;
  Uint8 y;
  bool joystickU;
  bool joystickD;
  bool joystickR;
  bool joystickL;
};
enum BlockType {
  BLOCK_NONE,
  BLOCK_GRASS_SOIL,
  BLOCK_GRASS_SOIL_RIGHT,
  BLOCK_GRASS_SOIL_LEFT,
  BLOCK_SOIL,
  BLOCK_IRON_GRAY,
  BLOCK_MAGICAL_BOX,
  BLOCK_MAX,
};

enum EVENT_BOX_STATE { EVENT_BOX_STATE_ACTIVE, EVENT_BOX_STATE_MAX };

std::vector<BlockData> blockData = {
    {BLOCK_NONE, "..", {1, 1}, NULL, {}},
    {BLOCK_GRASS_SOIL, "g0", {1, 1}, "img/block/grass-soil.png", {}},
    {BLOCK_GRASS_SOIL_RIGHT,
     "gr",
     {1, 1},
     "img/block/grass-soil-right.png",
     {}},
    {BLOCK_GRASS_SOIL_LEFT, "gl", {1, 1}, "img/block/grass-soil-left.png", {}},
    {BLOCK_SOIL, "g1", {1, 1}, "img/block/soil.png", {}},
    {BLOCK_IRON_GRAY, "b0", {1, 1}, "img/block/iron-gray.png", {}},
    {BLOCK_MAGICAL_BOX,
     "b1",
     {6, 1},
     "img/block/event-box.png",
     {{EVENT_BOX_STATE_ACTIVE, {0, 1, 2, 3, 4, 5}, 6, true}}}};

struct {
  int up;
  int down;
  int right;
  int left;
  int jump;
} op;

enum HERO_STATE {
  HERO_STATE_STAND,
  HERO_STATE_CROUCH,
  HERO_STATE_WALK,
  HERO_STATE_RUN,
  HERO_STATE_JUMP,
  HERO_STATE_MAX
};

// ####################
//       main()
// ####################
int main(int argc, char* argv[]) {
  const int JOYSTICK_MOVEMENT_THREADHOLD = 16000;
  int quit = false;
  int frameCount = 0;
  int framePerSec = 0;
  int framePerSecCounter = 0;
  const int GRAVITY = SDL_STANDARD_GRAVITY;

  // Init
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error\n");
  }
  if (TTF_Init() < 0) {
    printf("error initializing SDL2_ttf\n");
  }
  if (SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0) {
    printf("error in SDL_GameControllerAddMappingsFromFile\n");
  }

  // Controller
  int nJoysticks = SDL_NumJoysticks();

  SDL_GameController* controller = SDL_GameControllerOpen(0);
  SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);

  // Window, Renderer
  SDL_Window* win =
      SDL_CreateWindow("sdl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  // Stage
  Stage stage(rend, &frameCount, SCREEN_HEIGHT, (char*)"stage/1-1.txt",
              blockData);
  xy camera = {0, 0};

  // Hero
  Character Hero(rend, &frameCount, (char*)"AA");
  Hero.setTextureByFile({4, 4}, (char*)"img/bool.png", 0.34);
  Hero.addAnm(HERO_STATE_STAND, {0, 1}, 48, true);
  Hero.addAnm(HERO_STATE_CROUCH, {12, 13, 14, 15}, 1, false);
  Hero.addAnm(HERO_STATE_WALK, {4, 5, 6, 7}, 4, true);
  Hero.addAnm(HERO_STATE_RUN, {4, 5, 6, 7}, 2, true);
  Hero.addAnm(HERO_STATE_JUMP, {8}, 6, false);
  Hero.speed = 90;
  Hero.jumpPower = 186;
  Hero.pos.x = 100;
  Hero.pos.y = 0;

  // Debugger
  Debugger debug(rend);
  debug.setFont("font/RobotoMono-Regular.ttf", 18);

  op = {0, 0, 0, 0, 0};

  Game game(60);

  // ####################
  //     Main Loop
  // ####################
  while (!quit) {
    SDL_Event event;
    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    // controller inputs
    CONTROLLER_STATE ctrl = {
        SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A),
        SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B),
        SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X),
        SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y),
        abs(SDL_JoystickGetAxis(joystick, 1)) > JOYSTICK_MOVEMENT_THREADHOLD &&
            SDL_JoystickGetAxis(joystick, 1) < 0,
        abs(SDL_JoystickGetAxis(joystick, 1)) > JOYSTICK_MOVEMENT_THREADHOLD &&
            SDL_JoystickGetAxis(joystick, 1) > 0,
        abs(SDL_JoystickGetAxis(joystick, 0)) > JOYSTICK_MOVEMENT_THREADHOLD &&
            SDL_JoystickGetAxis(joystick, 0) > 0,
        abs(SDL_JoystickGetAxis(joystick, 0)) > JOYSTICK_MOVEMENT_THREADHOLD &&
            SDL_JoystickGetAxis(joystick, 0) < 0,
    };

    // operation frame counts
    op.up = keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] || ctrl.joystickU
                ? op.up + 1
                : 0;
    op.down = keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] || ctrl.joystickD
                  ? op.down + 1
                  : 0;
    op.right =
        keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] || ctrl.joystickR
            ? op.right + 1
            : 0;
    op.left = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT] || ctrl.joystickL
                  ? op.left + 1
                  : 0;
    op.jump = keys[SDL_SCANCODE_SPACE] || ctrl.a ? op.jump + 1 : 0;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        default:
          break;
      }
    }

    /* VELOCITY */

    // X velocity
    if (op.right > 0 && (op.left == 0 || op.right < op.left)) {
      Hero.vel.x = ctrl.x ? Hero.speed * 1.5 : Hero.speed;
      Hero.flip.x = false;
    } else if (op.left > 0 && (op.right == 0 || op.right > op.left)) {
      Hero.vel.x = ctrl.x ? -Hero.speed * 1.5 : -Hero.speed;
      Hero.flip.x = true;
    } else {
      Hero.vel.x = 0;
    }

    // Y velocity
    Hero.vel.y += GRAVITY;
    if (op.jump == 1 && Hero.onGround) {
      Hero.vel.y = -Hero.jumpPower;
    }

    Hero.onGround = false;
    // Hero.onWall = false;
    //  Intersection
    for (int i = 0; i < stage.stageRow; i++)
      for (int j = 0; j < stage.stageCol; j++) {
        Texture* block = stage.blocks[i][j];
        if (block == nullptr) continue;
        SDL_FRect tmpRectX = {Hero.dstRect.x + Hero.vel.x / 10, Hero.dstRect.y,
                              Hero.dstRect.w, Hero.dstRect.h};
        SDL_FRect tmpRectY = {Hero.dstRect.x, Hero.dstRect.y + Hero.vel.y / 10,
                              Hero.dstRect.w, Hero.dstRect.h};
        // x
        if (SDL_HasIntersectionF(&block->dstRect, &tmpRectX)) {
          if (Hero.vel.x > 0)
            Hero.pos.x = block->pos.x - Hero.getW();
          else if (Hero.vel.x < 0)
            Hero.pos.x = block->pos.x + block->getW();
          Hero.vel.x = 0;
          Hero.onWall = true;
        }
        // y
        if (SDL_HasIntersectionF(&block->dstRect, &tmpRectY)) {
          if (Hero.vel.y > 0) {
            Hero.pos.y = block->pos.y - Hero.getH();
            Hero.onGround = true;
          } else if (Hero.vel.y < 0)
            Hero.pos.y = block->pos.y + block->getH();
          Hero.vel.y = 0;
        }
      }

    // Add vel
    Hero.pos.x += Hero.vel.x / 10;
    Hero.pos.y += Hero.vel.y / 10;

    // Border
    if (Hero.pos.y > SCREEN_HEIGHT + Hero.getH() / 2) {
      Hero.pos.x = 100;
      Hero.pos.y = 500;
    }
    if (Hero.pos.x < 0) {
      Hero.pos.x = 0;
    }

    // Calc drawPos
    if (Hero.pos.x >= (SCREEN_WIDTH - Hero.getW()) / 2) {
      Hero.drawPos.x = (SCREEN_WIDTH - Hero.getW()) / 2;
      camera.x = Hero.pos.x - (SCREEN_WIDTH - Hero.getW()) / 2;
    } else {
      Hero.drawPos.x = Hero.pos.x;
      camera.x = 0;
    }
    if (Hero.pos.y <= (SCREEN_HEIGHT * 0.2)) {
      Hero.drawPos.y = SCREEN_HEIGHT * 0.2;
      camera.y = Hero.pos.y - SCREEN_HEIGHT * 0.2;
    } else {
      Hero.drawPos.y = Hero.pos.y;
      camera.y = 0;
    }

    // Set Hero state
    if (Hero.onGround && op.down > 0) {
      Hero.state = HERO_STATE_CROUCH;
    } else if (Hero.onGround && op.right > 0) {
      Hero.state = ctrl.x ? HERO_STATE_RUN : HERO_STATE_WALK;
    } else if (Hero.onGround && op.left > 0) {
      Hero.state = ctrl.x ? HERO_STATE_RUN : HERO_STATE_WALK;
    } else if (Hero.onGround) {
      Hero.state = HERO_STATE_STAND;
    } else if (!Hero.onGround) {
      Hero.state = HERO_STATE_JUMP;
    }

    // ####################
    //        Draw
    // ####################
    SDL_SetRenderDrawColor(rend, 99, 173, 236, 255);
    SDL_RenderClear(rend);

    // Draw Stage
    stage.draw(camera);

    // Draw Hero
    Hero.drawAnm();

    // Draw debug info
    debug.drawText(0, 0, "%.0ffps", game.currentFps);
    debug.drawText(0, 1, "%d", game.frameCount);
    debug.drawText(0, 2, "Pos x:%.2f y:%.2f", Hero.pos.x, Hero.pos.y);
    debug.drawText(0, 3, "Vel x:%.2f y:%.2f", Hero.vel.x, Hero.vel.y);
    debug.drawText(0, 4, "onG %d", Hero.onGround);
    debug.drawText(0, 5, "Btn  %s%s%s%s", ctrl.a ? "a " : "",
                   ctrl.b ? "b " : "", ctrl.x ? "x " : "", ctrl.y ? "y" : "");
    debug.drawText(0, 6, "Stk x:%.2d y:%.2d", SDL_JoystickGetAxis(joystick, 0),
                   SDL_JoystickGetAxis(joystick, 1));

    SDL_RenderPresent(rend);

    // Final
    framePerSecCounter++;
    frameCount++;
    game.calc();
    SDL_Delay(1000 / game.defaultFps);
  }
  stage.clear();
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}