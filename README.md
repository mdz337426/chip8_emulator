# CHIP-8 Emulator 🕹️

This is a simple CHIP-8 Emulator built in **C++** using **SDL2** for rendering graphics. It emulates the classic CHIP-8 virtual machine that was used in the 1970s and can run original CHIP-8 ROMs like Pong, Tetris, and more.

---

## Features ✅

- 64x32 pixel monochrome display
- Fully functional CHIP-8 instruction set
- Graphics rendering using SDL2
- Hex-based keypad input support
- Delay and sound timer emulation
- Simple and clean C++ class-based design
- Sprite drawing with collision detection
- ROM loading from file

---

## Requirements ⚙️

- C++ compiler (g++, clang++, etc.)
- SDL2 development libraries

### Install SDL2

**Ubuntu/Debian:**
```bash
sudo apt install libsdl2-dev
```


## Build and Run 🚀
```
g++ main.cpp chip8.cpp -lSDL2 -o chip8
./chip8 path/to/ROM.ch8
```


