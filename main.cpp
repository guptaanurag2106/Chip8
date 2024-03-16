#include <string.h>

#include <iostream>

#include "chip8.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: chip8 --rom <rom>" << std::endl;
    return 0;
  }
  bool DEBUG_MODE = true;
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
  CHIP8 chip8;
  chip8.loadROM(filename);

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
        result = chip8.runCycle();
      } else if (command == "print") {
        chip8.dumpRegisters();
      } else if (command == "exit") {
        result = false;
      } else if (command == "draw") {
        chip8.debugDraw();
      } else {
        std::cout << "Unknown command" << std::endl;
      }

    } else {
      result = chip8.runCycle();
    }
    if (!result) {
      break;
    }
  }

  return 0;
}