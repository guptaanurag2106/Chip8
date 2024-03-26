#pragma once

#include <SDL2/SDL.h>

#include "chip8.h"

bool sdlInit();
bool sdlRun(CHIP8* chip8);
void sdlCleanup();
void sdlDraw(CHIP8* chip8);

const uint8_t keymap[KEYBOARD_SIZE] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v,
};