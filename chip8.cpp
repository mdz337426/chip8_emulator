#include "chip8.hpp"
#include <cstring>
#include<fstream>
#include<iostream>
#include<cstdint>



Chip8::Chip8()
{
    Initialize();
}

void Chip8::Initialize() {
    pc     = 0x200;  // Program counter starts at 0x200
    opcode = 0;
    I      = 0;
    sp     = 0;

    // Clear gfx
    memset(gfx, 0, sizeof(gfx));
    drawFlag = false;

    // Clear stack, registers, and memory
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    memset(memory, 0, sizeof(memory));

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    // Load fontset...
}

bool Chip8::LoadROM(const char * filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        std::cerr<<"Failed to open open ROM:"<<filename<<std::endl;
        return false;
    }


    std::streamsize size = file.tellg();
    char * buffer = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    if(( static_cast<long>(size) + 0x200) > 4096)
    {
        std::cerr<<"ROM too large to fit in memory.\n";
        delete [] buffer;
        return false;
    }

    for(long i =0 ; i<size; i++)
    {
        memory[0x200+i] = buffer[i];
    }

    delete [] buffer;
    return true;
}

void Chip8::EmulateCycle() {
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x00FF) {
        case 0x00E0:
            memset(gfx, 0, 64 * 32);
            pc += 2;
            break;
        case 0x00EE:
            --sp;
            pc = stack[sp];
            pc += 2;
            break;
        default:
            std::cerr << "Unknown opcode 0x" << std::hex << opcode << std::endl;
            pc += 2;
        }
        break;
    case 0x1000:
        pc = opcode & 0x0FFF;
        break;
    case 0x2000:
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
        break;
    case 0x3000:
        pc += (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) ? 4 : 2;
        break;
    case 0x4000:
        pc += (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) ? 4 : 2;
        break;
    case 0x5000:
        pc += (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) ? 4 : 2;
        break;
    case 0x6000:
        V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        pc += 2;
        break;
    case 0x7000:
        V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        pc += 2;
        break;
    case 0x8000:
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            switch (opcode & 0x000F) {
            case 0x0:
                V[x] = V[y];
                break;
            case 0x1:
                V[x] |= V[y];
                break;
            case 0x2:
                V[x] &= V[y];
                break;
            case 0x3:
                V[x] ^= V[y];
                break;
            case 0x4:
                V[0xF] = (V[x] + V[y] > 255);
                V[x] += V[y];
                break;
            case 0x5:
                V[0xF] = V[x] > V[y];
                V[x] -= V[y];
                break;
            case 0x6:
                V[0xF] = V[x] & 1;
                V[x] >>= 1;
                break;
            case 0x7:
                V[0xF] = V[y] > V[x];
                V[x] = V[y] - V[x];
                break;
            case 0xE:
                V[0xF] = (V[x] & 0x80) >> 7;
                V[x] <<= 1;
                break;
            }
            pc += 2;
        }
        break;
    case 0x9000:
        pc += (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) ? 4 : 2;
        break;
    case 0xA000:
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    case 0xB000:
        pc = (opcode & 0x0FFF) + V[0];
        break;
    case 0xC000:
        V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
        pc += 2;
        break;
    case 0xD000:
        {
            uint8_t x = V[(opcode & 0x0F00) >> 8];
            uint8_t y = V[(opcode & 0x00F0) >> 4];
            uint8_t height = opcode & 0x000F;
            V[0xF] = 0;
            for (int row = 0; row < height; ++row) {
                uint8_t sprite = memory[I + row];
                for (int col = 0; col < 8; ++col) {
                    if ((sprite & (0x80 >> col)) != 0) {
                        int index = ((x + col) % 64) + ((y + row) % 32) * 64;
                        if (gfx[index] == 1)
                            V[0xF] = 1;
                        gfx[index] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
        }
        break;
    case 0xE000:
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            switch (opcode & 0x00FF) {
            case 0x9E:
                pc += keypad[V[x]] ? 2 : 0;
                break;
            case 0xA1:
                pc += !keypad[V[x]] ? 2 : 0;
                break;
            }
            pc += 2;
        }
        break;
    case 0xF000:
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            switch (opcode & 0x00FF) {
            case 0x07:
                V[x] = delay_timer;
                break;
            case 0x0A:
                // Wait for key press
                for (int i = 0; i < 16; ++i) {
                    if (keypad[i]) {
                        V[x] = i;
                        pc += 2;
                        break;
                    }
                }
                return; // Wait until key press
            case 0x15:
                delay_timer = V[x];
                break;
            case 0x18:
                sound_timer = V[x];
                break;
            case 0x1E:
                I += V[x];
                break;
            case 0x29:
                I = V[x] * 5; // font character location
                break;
            case 0x33:
                memory[I] = V[x] / 100;
                memory[I + 1] = (V[x] / 10) % 10;
                memory[I + 2] = V[x] % 10;
                break;
            case 0x55:
                for (int i = 0; i <= x; ++i)
                    memory[I + i] = V[i];
                break;
            case 0x65:
                for (int i = 0; i <= x; ++i)
                    V[i] = memory[I + i];
                break;
            }
            pc += 2;
        }
        break;
    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
        pc += 2;
        break;
    }

    // Update timers
    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) {
        if (sound_timer == 1) std::cout << "BEEP!\n";
        --sound_timer;
    }
}
