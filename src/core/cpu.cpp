#include "cpu.h"
#include "utils.h"

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<fstream>
#include<iostream>

CPU::CPU() :
    IStable{ &CPU::OP_IStable0,&CPU::OP_1nnn,&CPU::OP_2nnn,&CPU::OP_3xkk,&CPU::OP_4xkk,&CPU::OP_5xy0,&CPU::OP_6xkk,&CPU::OP_7xkk,
    &CPU::OP_IStable8,&CPU::OP_9xy0,&CPU::OP_Annn,&CPU::OP_Bnnn,&CPU::OP_Cxkk,&CPU::OP_Dxyn,&CPU::OP_IStableE,&CPU::OP_IStableF }
{
    //Initializing the instruction table
    for (int i = 0;i <= 0xE;i++) {
        IStable0[i] = &CPU::OP_NULL;
        IStable8[i] = &CPU::OP_NULL;
        IStableE[i] = &CPU::OP_NULL;
    }

    for (int i = 0;i <= 0x65;i++) {
        IStableF[i] = &CPU::OP_NULL;
    }

    IStable0[0x0] = &CPU::OP_00E0;
    IStable0[0xE] = &CPU::OP_00EE;

    IStable8[0x0] = &CPU::OP_8xy0;
    IStable8[0x1] = &CPU::OP_8xy1;
    IStable8[0x2] = &CPU::OP_8xy2;
    IStable8[0x3] = &CPU::OP_8xy3;
    IStable8[0x4] = &CPU::OP_8xy4;
    IStable8[0x5] = &CPU::OP_8xy5;
    IStable8[0x6] = &CPU::OP_8xy6;
    IStable8[0x7] = &CPU::OP_8xy7;
    IStable8[0xE] = &CPU::OP_8xyE;

    IStableE[0x0] = &CPU::OP_ExA1;
    IStableE[0xE] = &CPU::OP_Ex9E;

    IStableF[0x07] = &CPU::OP_Fx07;
    IStableF[0x0A] = &CPU::OP_Fx0A;
    IStableF[0x15] = &CPU::OP_Fx15;
    IStableF[0x18] = &CPU::OP_Fx18;
    IStableF[0x1E] = &CPU::OP_Fx1E;
    IStableF[0x29] = &CPU::OP_Fx29;
    IStableF[0x33] = &CPU::OP_Fx33;
    IStableF[0x55] = &CPU::OP_Fx55;
    IStableF[0x65] = &CPU::OP_Fx65;

    memory = new uint8_t[4096];
    memcpy(memory + 0x50, Presets::fontset, sizeof(Presets::fontset));
    memset(video_buffer, 0, sizeof(video_buffer));
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    pc = 0x200;
    drawFlag = 0;
}

void CPU::load(char* buffer, std::size_t buffer_size) {
    // Load the ROM contents into the CPU's memory, starting at 0x200
    memcpy(memory + 0x200, buffer, buffer_size);
}

void CPU::Cycle() {
    opcode = (memory[pc] << 8) | (memory[pc + 1]);
    pc += 2;
    //std::cout << opcode <<"\n";
    ((*this).*(IStable[(opcode & 0xF000) >> 12]))();
    // Decrement the delay timer if it's been set
    if (delayTimer > 0) {
        --delayTimer;
    }

    // Decrement the sound timer if it's been set
    if (soundTimer > 0) {
        --soundTimer;
    }
}

//Instuctions
void CPU::OP_IStable0() {
    ((*this).*(IStable0[(opcode & 0x000F)]))();
}

void CPU::OP_IStable8() {
    ((*this).*(IStable8[(opcode & 0x000F)]))();
}

void CPU::OP_IStableE() {
    ((*this).*(IStableE[(opcode & 0x000F)]))();
}

void CPU::OP_IStableF() {
    ((*this).*(IStableF[(opcode & 0x00FF)]))();
}

void CPU::OP_NULL() {
    //Do nothing
}

void CPU::OP_00E0() {
    memset(video_buffer, 0, sizeof(video_buffer));
    drawFlag = 1;
}

void CPU::OP_00EE() {
    sp--;
    pc = stack[sp];
}

void CPU::OP_1nnn() {
    pc = opcode & 0x0FFF;
}

void CPU::OP_2nnn() {
    stack[sp++] = pc;
    pc = opcode & 0xFFF;
}

void CPU::OP_3xkk() {
    if (V[(opcode & 0xF00) >> 8] == (opcode & 0xFF)) {
        pc += 2;
    }
}
void CPU::OP_4xkk() {
    if (V[(opcode & 0xF00) >> 8] != (opcode & 0xFF)) {
        pc += 2;
    }
}
void CPU::OP_5xy0() {
    if (V[(opcode & 0xF00) >> 8] == V[(opcode & 0x0F0)>>8]) {
        pc += 2;
    }
}

void CPU::OP_6xkk() {
    V[(opcode & 0x0F00) >> 8] = (opcode & 0xFF);
}
void CPU::OP_7xkk() {
    V[(opcode & 0x0F00) >> 8] += (opcode & 0xFF);
}

void CPU::OP_8xy0() {
    V[(opcode & 0xF00) >> 8] = V[(opcode & 0x0F0) >> 8];
}
void CPU::OP_8xy1() {
    V[(opcode & 0xF00) >> 8] |= V[(opcode & 0x0F0) >> 8];
}
void CPU::OP_8xy2() {
    V[(opcode & 0xF00) >> 8] &= V[(opcode & 0x0F0) >> 8];
}
void CPU::OP_8xy3() {
    V[(opcode & 0xF00) >> 8] ^= V[(opcode & 0x0F0) >> 8];
}
void CPU::OP_8xy4() {}
void CPU::OP_8xy5() {}
void CPU::OP_8xy6() {}
void CPU::OP_8xy7() {}
void CPU::OP_8xyE() {}

void CPU::OP_9xy0() {
    if (V[(opcode & 0xF00) >> 8] != V[(opcode & 0x0F0)>>8]) {
        pc += 2;
    }
}

void CPU::OP_Annn() {
    I = (opcode & 0xFFF);
}

void CPU::OP_Bnnn() {}
void CPU::OP_Cxkk() {}

void CPU::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint8_t length = opcode & 0x000F;
    char sprite[0xF];
    memcpy(sprite, memory + I, length);
    V[0xF] = 0;
    drawFlag = 1;
    // Wrap if going beyond screen boundaries
    uint8_t xPos = V[Vx];
    uint8_t yPos = V[Vy];

    for (int y = 0; y < length; y++) {
        for (int x = 0; x < 8; x++) {
            if (sprite[y] & (0x80 >> x)) {
                uint32_t* screenPixel =
                    &video_buffer[(((yPos + y) % VIDEO_BUFFER_HEIGHT) * VIDEO_BUFFER_WIDTH) + ((xPos + x) % VIDEO_BUFFER_WIDTH)];
                //
                if (*screenPixel == 0xFFFFFFFF) {
                    V[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void CPU::OP_Ex9E() {}
void CPU::OP_ExA1() {}

void CPU::OP_Fx07() {}
void CPU::OP_Fx0A() {}
void CPU::OP_Fx15() {}
void CPU::OP_Fx18() {}
void CPU::OP_Fx1E() {}
void CPU::OP_Fx29() {}
void CPU::OP_Fx33() {}
void CPU::OP_Fx55() {}
void CPU::OP_Fx65() {}