#include "chip8.h"

#include <fstream>
#include <iostream>

CHIP8::CHIP8() {
  mPC = 0x200;
  mVF = 0;
  mI = 0;
  mdelay_timer = 0;
  msound_timer = 0;

  std::fill(std::begin(mmemory), std::end(mmemory), 0);
  std::fill(std::begin(mregisters), std::end(mregisters), 0);
  std::fill(std::begin(mstack), std::end(mstack), 0);
  std::copy(std::begin(chip8_fontset), std::end(chip8_fontset),
            std::begin(mmemory) + FONTSET_ADDRESS);
  std::fill(&mdisp[0][0], &mdisp[0][0] + sizeof(mdisp), 0);
}

bool CHIP8::runCycle() {
  uint16_t ins = (mmemory[mPC++] << 8) | mmemory[mPC];
  std::cout << "PC: " << mPC << " - " << ins << std::endl;
  uint8_t opcode = (ins & 0xF000) >> 12;
  uint8_t x = (ins >> 8) & 0x000F;
  uint8_t y = (ins >> 4) & 0x000F;
  uint8_t n = ins & 0x000F;
  uint8_t kk = ins & 0x00FF;
  uint16_t nnn = ins & 0x0FFF;

  switch (opcode) {
    case 0x0:

      break;

    case 0x1:

      break;
    case 0x2:

      break;
    case 0x3:

      break;
    case 0x4:

      break;
    case 0x5:

      break;
    case 0x6:

      break;
    case 0x7:

      break;
    case 0x8:

      break;
    case 0x9:

      break;
    case 0xA:

      break;
    case 0xB:

      break;
    case 0xC:

      break;
    case 0xD:

      break;
    case 0xE:

      break;
    case 0xF:

      break;
    default:
      break;
  }
  return true;
}

void CHIP8::loadROM(const std::string &filename) {
  // load the rom into the memory
  std::ifstream rom;
  rom.open(filename, std::ios::in | std::ios::binary);
  if (!rom.is_open()) {
    std::cerr << "Could not load file: " << filename << std::endl;
    exit(1);
  }
  int pos = ROM_START;
  //   streampos size;
  //   char* memblock;
  //   size = rom.tellg();
  //     memblock = new char[size];
  //     rom.seekg(0, std::ios::beg);
  //     rom.read(memblock, size);

  //     delete[] memblock;
  uint8_t curr;
  while (rom.good()) {
    rom >> curr;
    if (pos >= MEM_SIZE) {
      std::cerr << "ROM too large";
      rom.close();
      break;
      //   exit(1);
    }
    mmemory[pos++] = curr;
  }
  std::cout << pos << std::endl;
  rom.close();
}

void CHIP8::dumpRegisters() {
  // print the registers and the PC and flags
  std::cout << "------------------------------------------------------\n";
  for (int i = 0; i < 16; ++i) {
    std::cout << " R" << i << ": " << std::hex << (int)mregisters[i] << " ";
  }
  std::cout << "PC: " << std::hex << mPC << " ";
  std::cout << std::dec;
  std::cout << "Flags: " << std::hex << (int)mVF << " " << std::endl;
  ;
  std::cout << "------------------------------------------------------"
            << std::endl;
}

void CHIP8::debugDraw() {
  for (int row = 0; row < DISP_ROW; ++row) {
    for (auto col : mdisp[row]) {
      if (!col) {
        std::cout << "0";
      } else {
        std::cout << " ";
      }
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}