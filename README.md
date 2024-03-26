# CHIP8 Emulator

## Installation

Before running the emulator, make sure you have SDL2 library installed on your system. Copy the `include` and `lib` directories to the `dependencies/sdl2` directory and the SDL2 dynamic link library (DLL), or .so to the root directory of the project.

## Usage

To build the emulator, navigate to the root directory of the project and run the following command:

```shell
make
```

Then run:

```shell
./CHIP8 --rom <path_to_rom>
```

## Controls

**Keyboard**: The CHIP8 system originally used a 16-key hexadecimal keypad, which is mapped to the following keys on your keyboard:

    1 2 3 4
    Q W E R
    A S D F
    Z X C V

These keys correspond to the CHIP8 keypad layout:

    1 2 3 C
    4 5 6 D
    7 8 9 E
    A 0 B F

The ROMS used in this work are from dmatlack's chip8 emulator available on GitHub [https://github.com/dmatlack/chip8](https://github.com/dmatlack/chip8)
