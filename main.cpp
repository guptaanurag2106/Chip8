#include <string.h>
#include <unistd.h>

#include <iostream>

#include "chip8.h"
#include "display.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: chip8 --rom <rom>" << std::endl;
    return 0;
  }

  bool DEBUG_MODE = false;
  bool audio = false;
  std::string filename;

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      std::cout << "Debug mode" << std::endl;
      DEBUG_MODE = true;
    }
    if (strcmp(argv[i], "-a") == 0) {
      std::cout << "Audio enabled" << std::endl;
      audio = true;
    }
    if (!strcmp(argv[i], "--rom") && (i + 1) < argc) filename = argv[i + 1];
  }

  if (DEBUG_MODE) {
    std::cout << "Running in DEBUG MODE" << std::endl;
  }

  CHIP8 chip8(audio);
  chip8.loadROM(filename);
  std::cout << "Successfully loaded ROM" << std::endl;

  bool init_success = sdlInit();
  if (!init_success) return 0;

  bool quit = false;

  auto time_prev = std::chrono::system_clock::now();
  while (!quit) {
    // std::string x;
    // std::cin >> x;

    bool result = chip8.runCycle();
    chip8.dumpRegisters();
    if (!result) {
      std::cout << "Could not process last instruction..." << std::endl;
      break;
    }

    if (chip8.draw_flag) {
      std::cout << "drawing" << std::endl;
      sdlDraw(&chip8);
      // chip8.debugDraw();
    }

    quit = sdlRun(&chip8);
    // if (chip8.key_pressed != -1) {
    //   std::cout << "Key pressed " << chip8.key_pressed << std::endl;
    // }

    auto time_now = std::chrono::system_clock::now();
    // std::chrono::duration<double> diff = time_now - time_prev;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_now - time_prev);

    if (milliseconds.count() >= CLOCK_RATE_MS) {
      // chip8.timerTick();
      // time_prev = time_now;
    }
    usleep(1500);
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