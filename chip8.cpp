#include "chip8.h"

#include <fstream>
#include <iostream>

CHIP8::CHIP8(bool audio) {
  audio_enable = audio;
  mSP = 0;
  mPC = ROM_START;
  mI = 0;
  mdelay_timer = 0;
  msound_timer = 0;
  draw_flag = true;

  std::fill(std::begin(key_pressed), std::end(key_pressed), 0);
  std::fill(std::begin(mstack), std::end(mstack), 0);
  std::fill(std::begin(mregisters), std::end(mregisters), 0);
  std::fill(std::begin(mmemory), std::end(mmemory), 0);
  std::fill(&mdisp[0][0], &mdisp[0][0] + sizeof(mdisp), 0);
  for (int i = FONTSET_ADDRESS; i < FONTSET_ADDRESS + 80; i++) {
    mmemory[i] = chip8_fontset[i];
  }
  srand(std::time(NULL));
}

// load the rom into the memory
void CHIP8::loadROM(const std::string &filename) {
  std::ifstream rom(filename, std::ios::binary);

  if (!rom.is_open()) {
    std::cerr << "Could not load file: " << filename << std::endl;
    exit(1);
  }

  char c;
  int i;
  for (i = ROM_START; rom.get(c); i++) {
    mmemory[i] = (uint8_t)c;
    if (i > MEM_SIZE) {
      std::cerr << "ROM too large, exceeding memory size" << std::endl;
      rom.close();
      exit(1);
    }
  }

  mmem_end = i;
  std::cout << "Memory size: " << mmem_end - ROM_START << " bytes" << std::endl;
  rom.close();
}

bool CHIP8::runCycle() {
  uint16_t ins = ((uint16_t)mmemory[mPC] << 8) | (uint16_t)mmemory[mPC + 1];

  uint8_t opcode = OPCODE(ins);
  uint8_t x = X(ins);
  uint8_t y = Y(ins);
  uint8_t n = N(ins);
  uint8_t nn = NN(ins);
  uint16_t nnn = NNN(ins);
  uint8_t X, Y;

  switch (opcode) {
    case 0x0:
      if (x == 0 && y == 0xE && n == 0) {  // 0x00E0: clear screen
        std::fill(&mdisp[0][0], &mdisp[0][0] + sizeof(mdisp), 0);
        draw_flag = true;
        mPC += 2;
      } else if (x == 0 && y == 0xE &&
                 n == 0xE) {  // 0x00EE: return from subroutine
        mPC = mstack[--mSP];
      } else {
        return false;
      }
      break;

    case 0x1:
      mPC = nnn;  // 0x0NNN: Jump to address NNN
      break;

    case 0x2:
      mstack[mSP++] = mPC + 2;
      mPC = nnn;
      break;

    case 0x3:
      mPC += (mregisters[x] == nn)
                 ? 4
                 : 2;  // 3XNN: Skip next instruction if VX == NN

      break;

    case 0x4:
      mPC += (mregisters[x] != nn)
                 ? 4
                 : 2;  // 4XNN: Skip next instruction if VX != NN
      break;

    case 0x5:
      if (n != 0) {
        return false;
      }
      // 5XY0: Skip next instruction if VX == VY
      mPC += (mregisters[x] == mregisters[y]) ? 4 : 2;
      break;

    case 0x6:
      mregisters[x] = nn;
      mPC += 2;
      break;

    case 0x7:
      mregisters[x] += nn;
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
          mregisters[15] =
              (((int)mregisters[x] + (int)mregisters[y]) > 255) ? 1 : 0;
          mregisters[x] += mregisters[y];  // 0x8XY4 VX=VX+VY
          break;
        case 5:
          mregisters[15] = (mregisters[x] > mregisters[y]) ? 1 : 0;
          mregisters[x] -= mregisters[y];  // 0x8XY5 VX=VX-VY
          break;
        case 6:
          // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
          // VX is shifted in place, Y is ignored
          // mregisters[x] = mregisters[y];

          mregisters[15] = mregisters[x] & 0x1;  // 0x8XY6 VX=VY>>1
          mregisters[x] = mregisters[x] >> 1;
          break;
        case 7:
          mregisters[15] = mregisters[y] > mregisters[x] ? 1 : 0;
          mregisters[x] = mregisters[y] - mregisters[x];  // 0x8XY5 VX=VY-VX
          break;
        case 0xE:
          // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
          // VX is shifted in place, Y is ignored
          // mregisters[x] = mregisters[y];

          mregisters[15] = mregisters[x] & 0x80;  // 0x8XYE VX=VY<<1
          mregisters[x] = mregisters[x] << 1;
          break;
        default:
          return false;
      }
      mPC += 2;
      break;

    case 0x9:
      if (n != 0) {
        return false;
      }
      // 9x9XY0: Skip next instruction if VX != VY
      mPC += mregisters[x] != mregisters[y] ? 4 : 2;
      break;

    case 0xA:
      mI = nnn;  // 0xANNN:  Set I to NNN
      mPC += 2;
      break;

    case 0xB:
      // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
      // 0xBXNN jumps to XNN + VX
      // mPC = mregisters[x] + nnn;
      mPC = mregisters[0] + nnn;  // 0xBNNN: Jump to NNN + V0
      break;

    case 0xC:
      mregisters[x] = (rand() % 256) & nn;  // 0xCXNN: Set VX to random NN
      mPC += 2;
      break;

    case 0xD:
      Y = mregisters[y] % DISP_ROW;
      X = mregisters[x] % DISP_COL;
      std::cout << (int)X << " " << (int)Y << std::endl;
      mregisters[15] = 0;

      for (int i = 0; i < n; i++) {
        if ((Y + i) > DISP_ROW - 1) break;
        uint8_t curr_data = mmemory[mI + i];

        // for (int j = 0; j < 8; j++) {
        //   if ((X + j) > DISP_COL - 1) break;
        //   if (curr_data & (0x80 >> j)) {
        //     if (mdisp[Y + i][X + j]) mregisters[15] = 1;
        //     mdisp[Y + i][X + j] = !mdisp[Y + i][X + j];
        //   }
        // }
        for (int j = 0; j < 8; j++) {
          // the value of the bit in the sprite
          uint8_t bit = (curr_data >> j) & 0x1;
          // the value of the current pixel on the screen
          bool *pixelp = &mdisp[(Y + i) % DISP_ROW][(X + (7 - j)) % DISP_COL];

          // if drawing to the screen would cause any pixel to be erased,
          // set the collision flag to 1
          if (bit == 1 && *pixelp == 1) mregisters[15] = 1;

          // draw this pixel by XOR
          *pixelp = *pixelp ^ bit;
        }
      }

      draw_flag = true;
      mPC += 2;
      break;

    case 0xE:
      if (nn == 0x9E) {
        mPC += (key_pressed[mregisters[x]]) ? 4 : 2;
      } else if (nn == 0xA1) {
        mPC += !key_pressed[mregisters[x]] ? 4 : 2;
      } else {
        return false;
      }
      break;

    case 0xF:
      if (nn == 0x07) {
        mregisters[x] = mdelay_timer;  // 0xFX07: VX = delay_timer
      } else if (nn == 0x0A) {
        for (auto key : key_pressed)
          if (key) {
            mPC += 2;
            break;
          }
        mPC -= 2;
        // 0xFX0A: wait for keypress (making sure that if no
        // key_pressed(-1) the program counter does not change)
      } else if (nn == 0x15) {
        mdelay_timer = mregisters[x];  // 0xFX15: delay_timer = VX
      } else if (nn == 0x18) {
        msound_timer = mregisters[x];  // 0xFX18: sound_timer = VX
      } else if (nn == 0x1E) {
        mregisters[15] = (mI + mregisters[x] > 0xfff) ? 1 : 0;
        mI += (uint16_t)mregisters[x];  // 0xFX1E: I = I + VX
      } else if (nn == 0x29) {
        mI = (uint16_t)mregisters[x] * FONTSET_BYTES_PER_CHAR +
             FONTSET_ADDRESS;  // 0xFX29: I = sprite_addr[Vx]
      } else if (nn == 0x33) {
        mmemory[mI] = (mregisters[x] % 1000) / 100;    // hundred's digit
        mmemory[mI + 1] = (mregisters[x] % 100) / 10;  // ten's digit
        mmemory[mI + 2] = (mregisters[x] % 10);        // one's digit

      } else if (nn == 0x55) {
        // ! Depends on implementation, starting with CHIP-48 and SUPER-CHIP,
        // mI was not changed, whereas previously new_mI = mI + x + 1
        for (uint16_t i = 0; i <= x; i++) {
          mmemory[mI + i] = mregisters[i];
        }
        mI = mI + x + 1;
      } else if (nn == 0x65) {
        std::cout << "copy " << std::hex << (int)mI << " " << (int)x
                  << std::endl;
        for (uint16_t i = 0; i <= x; i++) {
          mregisters[i] = mmemory[mI + i];
        }
        mI = mI + x + 1;
      } else {
        return false;
      }
      mPC += 2;
      break;

    default:
      break;
  }
  timerTick();
  return true;
}

void CHIP8::timerTick() {
  if (mdelay_timer > 0) {
    --mdelay_timer;
  }
  if (msound_timer > 0) {
    --msound_timer;
  }
  if (msound_timer == 0 && audio_enable) {
    std::cout << "\a" << std::endl;
  }
}

void CHIP8::dumpRegisters() {
  // print the registers and the PC and flags
  std::cout << "---------------------------------------------------------------"
               "-------------------------------------------------------"
               "-------------"
            << std::endl;
  std::cout << "PC: " << std::hex << mPC << " ";
  for (int i = 0; i < 16; ++i) {
    std::cout << " R" << i << ": " << (int)mregisters[i] << " ";
  }
  std::cout << "I: " << (int)mI << std::endl;
  std::cout << "---------------------------------------------------------------"
               "-------------------------------------------------------"
               "-------------"
            << std::endl;
}

void CHIP8::debugDraw() {
  for (int row = 0; row < DISP_ROW; ++row) {
    for (auto col : mdisp[row]) {
      if (!col) {
        std::cout << " ";
      } else {
        std::cout << "O";
      }
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}