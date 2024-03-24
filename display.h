#pragma once

#include <SDL2/SDL.h>

#include "chip8.h"

bool sdlInit();
bool sdlRun(CHIP8* chip8);
void sdlCleanup();
void sdlDraw(const CHIP8* chip8);