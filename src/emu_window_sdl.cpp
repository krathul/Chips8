#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<vector>

//#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "emu_window_sdl.h"
#include "InputMapping.h"
#include "Chip8.h"

Emu_window::Emu_window(Chip8_System* system_)
    : System{ system_ } {
    System->Initialize();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL2 %s", SDL_GetError());
        exit(1);
    }
    //SDL_SetMainReady();
}

void Emu_window::OnStart() {
    SCREEN_HEIGHT = 320;
    SCREEN_WIDTH = 640;
    const std::string window_title = "Chips8";
    render_Window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(render_Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    gTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, System->VIDEO_BUFFER_WIDTH, System->VIDEO_BUFFER_HEIGHT);
    is_open = true;
    paused = false;
}

void Emu_window::Run() {
    if (!paused) {
        System->run();
        OnUpdate();
        PollEvents();
    }
}

bool Emu_window::IsOpen() {
    return is_open;
}

void Emu_window::OnUpdate() {
    int pitch = sizeof(uint32_t) * System->VIDEO_BUFFER_WIDTH;
    SDL_UpdateTexture(gTexture, NULL, System->Video_Buffer, pitch);
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);
}

Emu_window::~Emu_window() {
    System->emulating = 0;
    SDL_Quit();
}

void Emu_window::OnKeyEvent(int key, uint8_t state) {
    if (state == SDL_PRESSED) {
        System->Update_Key(key, 1);
    }
    else if (state == SDL_RELEASED) {
        System->Update_Key(key, 0);
    }
}

void Emu_window::PollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type) {
        case SDL_QUIT:
            is_open = false;
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            OnKeyEvent(InputProcessKeypadEvent(event.key.keysym.sym), event.key.state);
            break;
        }
    }
}

void Emu_window::WaitEvents() {
    SDL_Event event;
    while (SDL_WaitEvent(&event) != 0) {
        switch (event.type) {
        case SDL_QUIT:
            is_open = false;
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            OnKeyEvent(InputProcessKeypadEvent(event.key.keysym.sym), event.key.state);
            break;
        }
    }
}