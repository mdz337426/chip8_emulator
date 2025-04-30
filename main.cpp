#include <SDL2/SDL.h>
#include "chip8.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

const int VIDEO_SCALE = 10;
const int VIDEO_WIDTH = 64;
const int VIDEO_HEIGHT = 32;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./chip8 <ROM>" << std::endl;
        return 1;
    }

    Chip8 chip8;

    if (!chip8.LoadROM(argv[1])) {
        std::cerr << "Failed to load ROM\n";
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          VIDEO_WIDTH * VIDEO_SCALE,
                                          VIDEO_HEIGHT * VIDEO_SCALE,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             VIDEO_WIDTH,
                                             VIDEO_HEIGHT);

    bool running = true;
    SDL_Event event;

    auto lastTimerUpdate = std::chrono::high_resolution_clock::now();

    while (running) {
        
        // Input (to be implemented)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                bool isPressed = (event.type == SDL_KEYDOWN);
                switch (event.key.keysym.sym) {
                    case SDLK_1: chip8.keypad[0x1] = isPressed; break;
                    case SDLK_2: chip8.keypad[0x2] = isPressed; break;
                    case SDLK_3: chip8.keypad[0x3] = isPressed; break;
                    case SDLK_4: chip8.keypad[0xC] = isPressed; break;
                    case SDLK_q: chip8.keypad[0x4] = isPressed; break;
                    case SDLK_w: chip8.keypad[0x5] = isPressed; break;
                    case SDLK_e: chip8.keypad[0x6] = isPressed; break;
                    case SDLK_r: chip8.keypad[0xD] = isPressed; break;
                    case SDLK_a: chip8.keypad[0x7] = isPressed; break;
                    case SDLK_s: chip8.keypad[0x8] = isPressed; break;
                    case SDLK_d: chip8.keypad[0x9] = isPressed; break;
                    case SDLK_f: chip8.keypad[0xE] = isPressed; break;
                    case SDLK_z: chip8.keypad[0xA] = isPressed; break;
                    case SDLK_x: chip8.keypad[0x0] = isPressed; break;
                    case SDLK_c: chip8.keypad[0xB] = isPressed; break;
                    case SDLK_v: chip8.keypad[0xF] = isPressed; break;
                }
            }
            
        }

        chip8.EmulateCycle();

        auto now = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float, std::chrono::milliseconds::period>(now - lastTimerUpdate).count();
    if (elapsed >= 1000.0f / 60.0f) {
        if (chip8.delay_timer > 0) chip8.delay_timer--;
        if (chip8.sound_timer > 0) chip8.sound_timer--;
        lastTimerUpdate = now;
    }


        if (chip8.drawFlag) {
            uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];

            for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; ++i) {
                pixels[i] = (chip8.gfx[i] ? 0xFFFFFFFF : 0x000000FF); // white or black
            }

            SDL_UpdateTexture(texture, nullptr, pixels, VIDEO_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);

            chip8.drawFlag = false;

        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
