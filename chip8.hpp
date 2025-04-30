#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>   // for uint8_t, uint16_t
#include <cstring>   // for memset


class Chip8{
    public:
    unsigned char delay_timer;
    unsigned char sound_timer;
    uint8_t gfx[64 * 32];  // Screen pixels: 64x32 resolution
    unsigned char keypad[16];
    bool drawFlag;
    Chip8();
    void Initialize();
    bool LoadROM(const char * filename);
    void EmulateCycle();

    private:
    uint16_t opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned short stack[16];
    unsigned short sp;
    
    

};

#endif