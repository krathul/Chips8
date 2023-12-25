#ifndef CHIP8_H
#define CHIP8_H

#include<SDL2/SDL.h>
#include<iostream>

#include "core/cpu.h"

class Chip8 {
private:
    CPU cpu;
    
public:
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    SDL_Texture* gTexture;

    int SCREEN_HEIGHT;
    int SCREEN_WIDTH;

    Chip8(int scale);
    bool Init();
    bool LoadRom(std::string path);
    void run();
    void Update();
    ~Chip8();
};

#endif
