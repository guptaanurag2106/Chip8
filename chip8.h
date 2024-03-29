#pragma once

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <stack>
#include <string>

#define MEM_SIZE 0x1000
#define ROM_START 0x200
#define FONTSET_ADDRESS 0x0
#define FONTSET_BYTES_PER_CHAR 5
#define STACK_SIZE 16
#define DISP_ROW 32
#define DISP_COL 64
#define CLOCK_RATE_MS (int)((1.0 / 60.0) * 1000 + 0.5)
#define KEYBOARD_SIZE 16

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
  CHIP8(bool audio);

  void loadROM(const std::string &filename);
  bool runCycle();
  void timerTick();
  void dumpRegisters();
  void debugDraw();
  uint8_t key_pressed[KEYBOARD_SIZE];
  bool audio_enable;
  bool draw_flag;
  bool mdisp[DISP_ROW][DISP_COL];

 private:
  uint8_t mmemory[MEM_SIZE];
  uint8_t mregisters[16];
  uint16_t mstack[STACK_SIZE];
  uint8_t mSP;
  unsigned int mmem_end;
  uint16_t mPC;
  uint16_t mI;
  uint8_t mdelay_timer;
  uint8_t msound_timer;
};
const uint8_t chip8_fontset[80] = {
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
