#include<SDL2/SDL.h>
#include<iostream>
#include<string>
#include<chrono>

#include "core/cpu.h"
#include "Chip8.h"

int main(int argc, char** argv) {
    if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
    char const* rom_path = argv[3];
    Chip8 emu(videoScale);
    emu.Init();
    emu.LoadRom(std::string(rom_path));

    //While application is running
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;
    SDL_Event e;
    while (!quit)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            emu.run();
        }
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
        }
    }
}
