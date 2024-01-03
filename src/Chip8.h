#pragma once

#include<cstdint>

#include "core/cpu.h"

class Chip8_System {
private:
    CPU cpu;
    uint8_t Memory[4096];
    uint8_t Keypad_Buffer[16];
    int Clear_Flag;

public:
    static const int VIDEO_BUFFER_WIDTH=64;
    static const int VIDEO_BUFFER_HEIGHT = 32;
    static const int Video_Buffer_Size = VIDEO_BUFFER_WIDTH * VIDEO_BUFFER_HEIGHT;

    int emulating;
    uint32_t Video_Buffer[Video_Buffer_Size];
    explicit Chip8_System();
    void Initialize();
    bool LoadRom(std::string path);
    void run();
    void Update_Key(int key,int state);
};

