#pragma once

#include <SDL2/SDL.h>

#include "Chip8.h"

class Emu_window {
private:
    Chip8_System* System;
    bool is_open;
    bool paused;
    SDL_Window* render_Window;
    SDL_Renderer* gRenderer;
    SDL_Texture* gTexture;
    int SCREEN_HEIGHT;
    int SCREEN_WIDTH;
    
public:
    explicit Emu_window(Chip8_System* system_);
    ~Emu_window();
    bool IsOpen();
    void Run();
    void OnUpdate();
    void OnStart();
    void PollEvents();
    void OnKeyEvent(int key, uint8_t state);
    void WaitEvents();
};