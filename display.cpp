#include "display.h"

#include <iostream>

static SDL_Renderer* renderer;
static SDL_Window* window;
static SDL_Texture* texture;

int disp_scaling = 10;

bool sdlInit() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
    return false;
  }
  std::cout << "asaaaaaaaaaaaaaaaaaaaaa" << disp_scaling << std::endl;
  // Create a window
  window = SDL_CreateWindow("CHIP 8", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, DISP_COL * disp_scaling,
                            DISP_ROW * disp_scaling, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return false;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == nullptr) {
    std::cerr << "Failed to create renderer" << SDL_GetError() << std::endl;
    return false;
  }
  SDL_RenderSetLogicalSize(renderer, DISP_COL * disp_scaling,
                           DISP_ROW * disp_scaling);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, DISP_COL, DISP_ROW);
  if (texture == nullptr) {
    std::cerr << "Error in setting up texture " << SDL_GetError() << std::endl;
    return false;
  }

  return true;
}

bool sdlRun(CHIP8* chip8) {
  // int key_pressed = -1;

  SDL_Event event;

  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return true;
    }
    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        return true;
      }

      for (int i = 0; i < KEYBOARD_SIZE; i++) {
        if (event.key.keysym.sym == keymap[i]) {
          chip8->key_pressed[i] = true;
          std::cout << std::hex << i << std::endl;
        }
      }
    }
  }
  return false;
}

void sdlDraw(CHIP8* chip8) {
  chip8->draw_flag = false;
  uint32_t pixels[DISP_ROW * DISP_COL];
  for (int i = 0; i < DISP_ROW; i++) {
    for (int j = 0; j < DISP_COL; j++) {
      if (chip8->mdisp[i][j]) {
        pixels[i * DISP_COL + j] = 0xFFFFFFFF;
      } else {
        pixels[i * DISP_COL + j] = 0xFF000000;
      }
    }
  }
  SDL_UpdateTexture(texture, NULL, pixels, DISP_COL * sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void sdlCleanup() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}