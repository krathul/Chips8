#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<cstdint>
#include<memory>

#include "core/cpu.h"
#include "core/utils.h"
#include "Chip8.h"


Chip8_System::Chip8_System()
    : cpu{ CPU(Memory, Keypad_Buffer, Video_Buffer, Clear_Flag) } {
    memset(Memory, 0, sizeof(Memory));
    memset(Keypad_Buffer, 0, sizeof(Keypad_Buffer));
    memset(Video_Buffer, 0, sizeof(Video_Buffer));
    Clear_Flag = 0;
    emulating = 0;
}

void Chip8_System::Initialize() {
    //Laod fonts into system memory
    memcpy(Memory + 0x50, Presets::fontset, sizeof(Presets::fontset));
    emulating = 1;
    //initialize cpu
    cpu.Init();
}

bool Chip8_System::LoadRom(std::string path) {
    // Open the file as a stream of binary and move the file pointer to the end
    std::FILE* rom = std::fopen(path.c_str(), "rb");

    if (rom!=NULL) {
        // Get size of file and allocate a buffer to hold the contents

        std::fseek(rom, 0, SEEK_END);
        std::size_t rom_size = ftell(rom);
        std::rewind(rom);

        char buffer[rom_size];
        std::fread(buffer, sizeof(char), rom_size, rom);
        std::fclose(rom);
        std::cerr << Memory;
        // Load the ROM contents into the system memory, starting at 0x200
        memcpy(Memory + 0x200, buffer, rom_size);
        return true;
    }
    else {
        return false;
    }
}

void Chip8_System::run() {
    cpu.Cycle();
    if (Clear_Flag == 1) {
        memset(Video_Buffer, 0, sizeof(uint32_t) * Video_Buffer_Size);
        Clear_Flag = 0;
    }
}

void Chip8_System::Update_Key(int key, int state) {
    Keypad_Buffer[key] = state;
}