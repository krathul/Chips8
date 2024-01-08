#include<iostream>
#include<fstream>
#include<string>
#include<chrono>
#include<memory>
#include<SDL2/SDL.h>

#include "emu_window_sdl.h"
#include "Chip8.h"

int main(int argc, char** argv) {
    char const* rom_path = argv[1];

    Chip8_System emu;
    emu.Initialize();
    emu.LoadRom(rom_path);

    std::unique_ptr<Emu_window> emu_window;
    emu_window = std::make_unique<Emu_window>(&emu);
    emu_window->OnStart();

    while (emu_window->IsOpen()) {
        emu_window->Run();
        SDL_Delay(10);
    }
}