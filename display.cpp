#include "display.h"

#include <iostream>

SDL_Renderer* renderer;
SDL_Window* window;

bool sdlInit() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
    return false;
  }

  // Create a window
  window = SDL_CreateWindow("CHIP 8", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 64 * 8, 32 * 8, 0);
  if (!window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return false;
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  return true;
}

bool sdlRun(CHIP8* chip8) {
  uint8_t key_pressed = -1;

  SDL_Event event;

  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return true;
    } else if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        return true;
      }

      switch (event.key.keysym.sym) {
        case SDLK_0:
          key_pressed = 0;
          break;
        case SDLK_1:
          key_pressed = 1;
          break;
        case SDLK_2:
          key_pressed = 2;
          break;
        case SDLK_3:
          key_pressed = 3;
          break;
        case SDLK_4:
          key_pressed = 4;
          break;
        case SDLK_5:
          key_pressed = 5;
          break;
        case SDLK_6:
          key_pressed = 6;
          break;
        case SDLK_7:
          key_pressed = 7;
          break;
        case SDLK_8:
          key_pressed = 8;
          break;
        case SDLK_9:
          key_pressed = 9;
          break;
        case SDLK_a:
          key_pressed = 0xA;
          break;
        case SDLK_b:
          key_pressed = 0xB;
          break;
        case SDLK_c:
          key_pressed = 0xC;
          break;
        case SDLK_d:
          key_pressed = 0xD;
          break;
        case SDLK_e:
          key_pressed = 0xE;
          break;
        case SDLK_f:
          key_pressed = 0xF;
          break;
        default:
          break;
      }
    }
  }
  chip8->key_pressed = key_pressed;
  return false;
}

void sdlDraw(const CHIP8* chip8) {}

void sdlCleanup() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}