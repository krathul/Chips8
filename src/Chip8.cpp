#include<SDL2/SDL.h>
#include<iostream>
#include<fstream>

#include "core/cpu.h"
#include "Chip8.h"


Chip8::Chip8(int scale) {
    cpu = CPU();
    gWindow = NULL;
    gRenderer = NULL;
    gTexture = NULL;
    SCREEN_HEIGHT = VIDEO_BUFFER_HEIGHT * scale;
    SCREEN_WIDTH = VIDEO_BUFFER_WIDTH * scale;
}

bool Chip8::Init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error %s\n", SDL_GetError());
        success = false;
    }
    else {
        gWindow = SDL_CreateWindow("CHIP-8-EMU", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL) {
                printf("Unable to create Renderer %s\n", SDL_GetError());
                success = false;
            }
            else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                gTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, VIDEO_BUFFER_WIDTH, VIDEO_BUFFER_HEIGHT);
                if (gTexture == NULL) {
                    printf("Unable to create Texture %s\n", SDL_GetError());
                    success = false;
                }
            }
        }
    }
    return success;
}

bool Chip8::LoadRom(std::string path) {
    // Open the file as a stream of binary and move the file pointer to the end
    std::FILE* rom = std::fopen(path.c_str(), "rb");

    if (rom!=NULL) {
        // Get size of file and allocate a buffer to hold the contents

        std::fseek(rom, 0, SEEK_END);
        std::size_t rom_size = ftell(rom);
        std::rewind(rom);
        char* buffer = new char[rom_size];

        std::fread(buffer, sizeof(char), rom_size, rom);
        std::fclose(rom);

        // Load the ROM contents into the CPU's memory, starting at 0x200
        cpu.load(buffer,rom_size);
        
        delete[] buffer;
        return true;
    }
    else {
        return false;
    }
}

void Chip8::run() {
    cpu.Cycle();

    if (cpu.drawFlag == 1) {
        Update();
    }
}

void Chip8::Update() {
    int pitch = sizeof(cpu.video_buffer[0]) * VIDEO_BUFFER_WIDTH;
    SDL_UpdateTexture(gTexture, NULL, cpu.video_buffer, pitch);
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);
}

Chip8::~Chip8() {
    //Free loaded image
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;

    //Destroy window    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}
