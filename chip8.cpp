#include "chip8.h"

#include <fstream>
#include <iostream>

CHIP8::CHIP8() {
  mPC = (uint8_t)0x200;
  mVF = 0;
  mI = 0;
  mdelay_timer = 0;
  msound_timer = 0;

  std::fill(std::begin(mmemory), std::end(mmemory), 0);
  std::fill(std::begin(mregisters), std::end(mregisters), 0);
  // std::fill(std::begin(mstack), std::end(mstack), 0);
  std::copy(std::begin(chip8_fontset), std::end(chip8_fontset),
            std::begin(mmemory) + FONTSET_ADDRESS);
  std::fill(&mdisp[0][0], &mdisp[0][0] + sizeof(mdisp), 0);

  srand(time(0));
}

void CHIP8::loadROM(const std::string &filename) {
  // load the rom into the memory
  std::ifstream rom(filename, std::ios::binary);

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

  while (rom.read(reinterpret_cast<char *>(&curr), 1)) {
    mmemory[pos++] = curr;
    if (pos >= MEM_SIZE) {
      std::cerr << "ROM too large, exceeding memory size" << std::endl;
      rom.close();
      exit(1);
    }
  }
  mmem_end = pos - 1;
  std::cout << "ROM successfully loaded" << std::endl;
  rom.close();
}

bool CHIP8::runCycle() {
  uint16_t ins = (mmemory[mPC] << 8) | mmemory[mPC + 1];
  // mPC++;
  std::cout << "PC: " << (int)mPC << " - " << ins << std::endl;

  uint8_t opcode = OPCODE(ins);
  uint8_t x = X(ins);
  uint8_t y = Y(ins);
  uint8_t n = N(ins);
  uint8_t nn = NN(ins);
  uint16_t nnn = NNN(ins);
  // std::cout << x << y << n << nn << nnn << std::endl;

  switch (opcode) {
    case 0x0:
      if (x == 0 && y == 0xE && n == 0) {  // 0x00E0: clear screen
        std::fill(&mdisp[0][0], &mdisp[0][0] + sizeof(mdisp), 0);
      } else if (x == 0 && y == 0xE &&
                 n == 0xE) {  // 0x00EE: return from subroutine
        mPC = mstack.top();
        mstack.pop();
      }
      mPC += 2;
      break;

    case 0x1:
      mPC = nnn;  // 0x0NNN: Jump to address NNN
      break;

    case 0x2:
      mstack.push(mPC);  // 0x2NNN: Call subroutine
      mPC = nnn;
      break;

    case 0x3:
      if (mregisters[x] == nn) {  // 3XNN: Skip next instruction if VX == NN
        mPC += 4;
      } else {
        mPC += 2;
      }
      break;

    case 0x4:
      if (mregisters[x] != nn) {  // 4XNN: Skip next instruction if VX != NN
        mPC += 4;
      } else {
        mPC += 2;
      }
      break;

    case 0x5:
      if (n != 0) {
        return false;
      }
      if (mregisters[x] ==
          mregisters[y]) {  // 5XY0: Skip next instruction if VX == VY
        mPC += 4;
      } else {
        mPC += 2;
      }
      break;

    case 0x6:
      if (x < 15) {  // 0x6XNN: Set VX to NN
        mregisters[x] = nn;
      } else if (x == 15) {
        mVF = nn;  // ?: can VF be changed by program
      }
      mPC += 2;
      break;

    case 0x7:
      if (x < 15) {  // 0x7XNN: VX += NN
        mregisters[x] += nn;
      }
      mPC += 2;
      break;

    case 0x8:
      switch (n) {
        case 0:
          mregisters[x] = mregisters[y];  // 0x8XY0 VX=VY
          break;
        case 1:
          mregisters[x] |= mregisters[y];  // 0x8XY1 VX=VX|VY
          break;
        case 2:
          mregisters[x] &= mregisters[y];  // 0x8XY2 VX=VX&VY
          break;
        case 3:
          mregisters[x] ^= mregisters[y];  // 0x8XY3 VX=VX^VY
          break;
        case 4:
          if (((int)mregisters[x] + (int)mregisters[y]) > 255) {  // TODO: check
            mVF = 1;
          } else {
            mVF = 0;
          }
          mregisters[x] += mregisters[y];  // 0x8XY4 VX=VX+VY
          break;
        case 5:
          if (mregisters[x] > -mregisters[y]) {  // TODO: check
            mVF = 1;
          } else {
            mVF = 0;
          }
          mregisters[x] -= mregisters[y];  // 0x8XY5 VX=VX-VY
          break;
        case 6:
          // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
          // VX is shifted in place, Y is ignored
          // mregisters[x] = mregisters[y];

          mVF = mregisters[x] & 0x1;  // 0x8XY6 VX=VY>>1
          mregisters[x] = mregisters[x] >> 1;
          break;
        case 7:
          if (mregisters[y] > -mregisters[x]) {  // TODO: check
            mVF = 1;
          } else {
            mVF = 0;
          }
          mregisters[x] = mregisters[y] - mregisters[x];  // 0x8XY5 VX=VY-VX
          break;
        case 0xE:
          // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
          // VX is shifted in place, Y is ignored
          // mregisters[x] = mregisters[y];

          mVF = mregisters[x] & 0x80;  // 0x8XYE VX=VY<<1
          mregisters[x] = mregisters[x] << 1;
        default:
          return false;
      }
      mPC += 2;
      break;

    case 0x9:
      if (n != 0) {
        return false;
      }
      if (mregisters[x] !=
          mregisters[y]) {  // 9x9XY0: Skip next instruction if VX != VY
        mPC += 4;
      } else {
        mPC += 2;
      }
      break;

    case 0xA:
      mI = nnn;  // 0xANNN:  Set I to NNN
      mPC += 2;
      break;

    case 0xB:
      // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
      // 0xBXNN jumps to XNN + VX

      mPC = mregisters[0] + nnn;  // 0xBNNN: Jump to NNN + V0
      break;

    case 0xC:
      mregisters[x] = (rand() % 256) & nn;  // 0xCXNN: Set VX to random NN
      mPC += 2;
      break;

    case 0xD:
      // TODO: DXYN
      break;

    case 0xE:
      if (nn == 0xA1) {
        // if (condn){ // TODO: check if key_pressed = mregisters[x]
        //   mPC += 4;
        // } else {
        //   mPC += 2;
        // }
      } else if (nn == 0x9E) {
        // if (condn){ // TODO: check if key_pressed != mregisters[x]
        //   mPC += 4;
        // } else {
        //   mPC += 2;
        // }
      } else {
        return false;
      }
      break;

    case 0xF:
      if (nn == 0x07) {
        mregisters[x] = mdelay_timer;  // 0xFX07: VX = delay_timer
      } else if (nn == 0x15) {
        mdelay_timer = mregisters[x];  // 0xFX15: delay_timer = VX
      } else if (nn == 0x18) {
        msound_timer = mregisters[x];  // 0xFX18: sound_timer = VX
      } else if (nn == 0x1E) {
        mI += mregisters[x];  // 0xFX1E: I = I + VX
      } else if (nn == 0x0A) {
        // if (key_pressed == 0){ // 0xFX0A: wait for keypress
        //   mPC -= 2;
        // }else{
        //   mregisters[x] = key_pressed;
        // }
      } else if (nn == 0x29) {
        // I = sprite_addr[Vx] //0xFX29: I = sprite_addr[Vx]
      } else if (nn == 0x33) {
        // uint8_t VX = mregisters[x];
        // mmemory[mI] = VX / 100;
        // VX %= 100;
        // mmemory[mI + 1] = VX / 10;
        // VX %= 10;
        // mmemory[mI + 2] = VX;
        mmemory[mI] = (mregisters[x] & 0xF00) >> 8;
        mmemory[mI + 1] = (mregisters[x] & 0x0F0) >> 4;
        mmemory[mI + 2] = mregisters[x] & 0x00F;
      } else if (nn == 0x55) {
        // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
        // mI was not changed, whereas previously new_mI = mI + x + 1
        for (int i = 0; i < x; i++) {
          mmemory[mI + i] = mregisters[i];
        }
      } else if (nn == 0x65) {
        for (int i = 0; i < x; i++) {
          mregisters[i] = mmemory[mI + i];
        }
      } else {
        return false;
      }
      mPC += 2;
      break;

    default:
      break;
  }
  return true;
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