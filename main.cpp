#include <SDL2/SDL.h>
#include <string.h>

#include <iostream>

#include "chip8.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: chip8 --rom <rom>" << std::endl;
    return 0;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
    return -1;
  }

  // Create a window
  SDL_Window* window =
      SDL_CreateWindow("SDL2 Test Window", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
    return -1;
  }
  // Wait for 5 seconds
  SDL_Delay(5000);

  // Clean up
  SDL_DestroyWindow(window);
  SDL_Quit();

  bool DEBUG_MODE = false;
  std::string filename;
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      std::cout << "Debug mode" << std::endl;
      DEBUG_MODE = true;
    }
    if (!strcmp(argv[i], "--rom") && (i + 1) < argc) filename = argv[i + 1];
  }
  std::cout << "Hello world" << std::endl;
  if (DEBUG_MODE) {
    std::cout << "Running in DEBUG MODE" << std::endl;
  }
  CHIP8* chip8 = new CHIP8();
  chip8->loadROM(filename);

  if (DEBUG_MODE) {
    std::cout << "Enter command (`r` to run the next cycle, `print` to print "
                 "state, `draw` to debug draw";
  }

  while (true) {
    bool result = true;
    if (DEBUG_MODE) {
      std::string command;
      std::cin >> command;
      if (command == "r") {
        result = chip8->runCycle();
      } else if (command == "print") {
        chip8->dumpRegisters();
      } else if (command == "exit") {
        result = false;
      } else if (command == "draw") {
        chip8->debugDraw();
      } else {
        std::cout << "Unknown command" << std::endl;
      }

    } else {
      result = chip8->runCycle();
    }
    if (!result) {
      std::cout << "Could not process last instruction..." << std::endl;
      break;
    }
  }

  delete chip8;

  return 0;
}