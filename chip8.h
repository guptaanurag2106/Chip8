#pragma once

#include <time.h>

#include <cstdint>
#include <cstdlib>
#include <stack>
#include <string>

#define MEM_SIZE 0x1000
#define ROM_START 0x200
#define FONTSET_ADDRESS 0x50
#define FONTSET_BYTES_PER_CHAR 5
#define STACK_SIZE 16
#define DISP_ROW 32
#define DISP_COL 64

#define OPCODE_MASK 0xF000
#define X_MASK 0x0F00
#define Y_MASK 0x00F0
#define N_MASK 0x000F
#define NN_MASK 0x00FF
#define NNN_MASK 0x0FFF

#define OPCODE(ins) ((ins & OPCODE_MASK) >> 12)
#define X(ins) ((ins & X_MASK) >> 8)
#define Y(ins) ((ins & Y_MASK) >> 4)
#define N(ins) (ins & N_MASK)
#define NN(ins) (ins & NN_MASK)
#define NNN(ins) (ins & NNN_MASK)

class CHIP8 {
 public:
  CHIP8();

  void loadROM(const std::string &filename);
  bool runCycle();
  void dumpRegisters();
  void debugDraw();

 private:
  bool mdisp[DISP_ROW][DISP_COL];
  uint8_t mmemory[MEM_SIZE];
  uint8_t mregisters[15];
  std::stack<uint16_t> mstack;
  unsigned int mmem_end;
  uint8_t mPC;
  uint8_t mI;
  uint8_t mVF;
  uint8_t mdelay_timer;
  uint8_t msound_timer;

  uint8_t chip8_fontset[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
      0x20, 0x60, 0x20, 0x20, 0x70,  // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
      0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
      0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
      0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
      0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
      0xF0, 0x80, 0xF0, 0x80, 0x80   // F
  };

  // An 8-bit delay timer which is decremented at a rate of 60 Hz (60 times per
  // second) until it reaches 0 An 8-bit sound timer which functions like the
  // delay timer, but which also gives off a beeping sound as long as it’s not 0
};