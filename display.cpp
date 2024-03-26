#include "display.h"

#include <iostream>

SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* texture;

bool sdlInit() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
    return false;
  }

  // Create a window
  window =
      SDL_CreateWindow("CHIP 8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       DISP_COL * 10, DISP_ROW * 10, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return false;
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Failed to create renderer" << SDL_GetError() << std::endl;
    return false;
  }
  SDL_RenderSetLogicalSize(renderer, DISP_COL * 10, DISP_ROW * 10);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, DISP_COL, DISP_ROW);
  if (texture == nullptr) {
    std::cerr << "Error in setting up texture " << SDL_GetError() << std::endl;
    return false;
  }

  return true;
}

bool sdlRun(CHIP8* chip8) {
  int key_pressed = -1;

  SDL_Event event;

  if (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return true;
    } else if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        return true;
      }

      switch (event.key.keysym.sym) {
        case SDLK_1:
          key_pressed = 0x1;
          break;
        case SDLK_2:
          key_pressed = 0x2;
          break;
        case SDLK_3:
          key_pressed = 0x3;
          break;
        case SDLK_4:
          key_pressed = 0xC;
          break;

        case SDLK_q:
          key_pressed = 0x4;
          break;
        case SDLK_w:
          key_pressed = 0x5;
          break;
        case SDLK_e:
          key_pressed = 0x6;
          break;
        case SDLK_r:
          key_pressed = 0xD;
          break;

        case SDLK_a:
          key_pressed = 0x7;
          break;
        case SDLK_s:
          key_pressed = 0x8;
          break;
        case SDLK_d:
          key_pressed = 0x9;
          break;
        case SDLK_f:
          key_pressed = 0xE;
          break;

        case SDLK_z:
          key_pressed = 0xA;
          break;
        case SDLK_x:
          key_pressed = 0x0;
          break;
        case SDLK_c:
          key_pressed = 0xB;
          break;
        case SDLK_v:
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

void sdlDraw(CHIP8* chip8) {
  // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  // SDL_RenderClear(renderer);
  // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  // for (int y = 0; y < DISP_ROW; y++) {
  //   for (int x = 0; x < DISP_COL; x++) {
  //     if (chip8->mdisp[y][x]) {
  //       SDL_Rect rect = {x * 10, y * 10, 10, 10};
  //       SDL_RenderFillRect(renderer, &rect);
  //     }
  //   }
  // }
  // SDL_RenderPresent(renderer);
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
  chip8->draw_flag = false;
}

void sdlCleanup() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}