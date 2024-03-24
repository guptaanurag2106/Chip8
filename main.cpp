// #include <SDL2/SDL.h>
#include <string.h>

#include <chrono>
#include <iostream>

#include "chip8.h"
#include "display.h"

int main(int argc, char* argv[]) {
  auto time_prev = std::chrono::system_clock::now();

  if (argc < 3) {
    std::cout << "Usage: chip8 --rom <rom>" << std::endl;
    return 0;
  }

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

  bool init_success = sdlInit();
  if (!init_success) {
    std::cerr << "Failed to initialize SDL" << std::endl;
    return 0;
  }

  bool quit = false;

  CHIP8 chip8;
  chip8.loadROM(filename);
  std::cout << "Successfully loaded ROM" << std::endl;
  while (!quit) {
    // bool result = chip8.runCycle();
    // if (!result) {
    //   std::cout << "Could not process last instruction..." << std::endl;
    //   break;
    // }

    if (chip8.draw_flag) {
      sdlDraw(&chip8);
    }

    quit = sdlRun(&chip8);
    if (chip8.key_pressed != -1) {
      std::cout << "Key pressed " << chip8.key_pressed << std::endl;
    }

    auto time_now = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = time_now - time_prev;

    if (diff.count() * 1000.0 > CLOCK_RATE_MS) {
      chip8.timerTick();
    }
    time_prev = time_now;
  }
  sdlCleanup();

  // if (DEBUG_MODE) {
  //   std::cout << "Enter command (`r` to run the next cycle, `print` to print
  //   "
  //                "state, `draw` to debug draw";
  // }

  // while (true) {
  //   bool result = true;
  //   if (DEBUG_MODE) {
  //     std::string command;
  //     std::cin >> command;
  //     if (command == "r") {
  //       result = chip8->runCycle();
  //     } else if (command == "print") {
  //       chip8->dumpRegisters();
  //     } else if (command == "exit") {
  //       result = false;
  //     } else if (command == "draw") {
  //       chip8->debugDraw();
  //     } else {
  //       std::cout << "Unknown command" << std::endl;
  //     }

  //   } else {
  //     result = chip8->runCycle();
  //   }
  // }

  return 0;
}