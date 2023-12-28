#include "cpu.h"
#include "utils.h"

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<fstream>
#include<iostream>
#include<random>

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
    memcpy(memory + Presets::font_offset, Presets::fontset, sizeof(Presets::fontset));
    memset(video_buffer, 0, sizeof(video_buffer));
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    memset(keypad_buffer, 0, sizeof(keypad_buffer));
    pc = 0x200;
    drawFlag = 0;

    //prng
    std::random_device seed;
    RANDOM_GENERATOR = std::mt19937(seed());
    PRN_DISTRIBUTE = std::uniform_int_distribution<int>(0, 255);
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
    pc = opcode & 0x0FFFU;
}

void CPU::OP_2nnn() {
    stack[sp++] = pc;
    pc = opcode & 0xFFFU;
}

void CPU::OP_3xkk() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    if (*Vx == (opcode & 0xFFU)) {
        pc += 2;
    }
}
void CPU::OP_4xkk() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    if (*Vx != (opcode & 0xFFU)) {
        pc += 2;
    }
}
void CPU::OP_5xy0() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    if (*Vx == *Vy) {
        pc += 2;
    }
}

void CPU::OP_6xkk() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    *Vx = (opcode & 0xFFU);
}

void CPU::OP_7xkk() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    *Vx += (opcode & 0xFFU);
}

void CPU::OP_8xy0() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx = *Vy;
}

void CPU::OP_8xy1() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx |= *Vy;
}

void CPU::OP_8xy2() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx &= *Vy;
}

void CPU::OP_8xy3() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx ^= *Vy;
}

void CPU::OP_8xy4() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    uint16_t sum = ( uint16_t ) (*Vx) + ( uint16_t ) (*Vy);
    if (sum > 255) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    *Vx = ( uint8_t ) (sum & 0xFFU);
}

void CPU::OP_8xy5() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    if (*Vx > *Vy) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    *Vx -= *Vy;
}

void CPU::OP_8xy6() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    //uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    V[0xF] = *Vx & 1U;
    *Vx >>= 1;
}

void CPU::OP_8xy7() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    if (*Vx < *Vy) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    *Vx = *Vy - *Vx;
}

void CPU::OP_8xyE() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    //uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    V[0xF] = (*Vx & 0x80U) >> 7;
    *Vx <<= 1;
}

void CPU::OP_9xy0() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    if (*Vx != *Vy) {
        pc += 2;
    }
}

void CPU::OP_Annn() {
    I = (opcode & 0xFFFU);
}

void CPU::OP_Bnnn() {
    pc = (opcode & 0x0FFFU) + V[0];
}

void CPU::OP_Cxkk() {
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    *Vx = (opcode & 0x0FFU) & PRN_DISTRIBUTE(RANDOM_GENERATOR);
}

void CPU::OP_Dxyn() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x00F0U) >> 4];
    uint8_t xPos = *Vx;
    uint8_t yPos = *Vy;

    uint8_t length = opcode & 0x000F;
    char sprite[0xF];
    memcpy(sprite, memory + I, length);
    V[0xF] = 0;
    drawFlag = 1;

    for (int y = 0; y < length; y++) {
        for (int x = 0; x < 8; x++) {
            if (sprite[y] & (0x80 >> x)) {
                // Wrap coordinates, if going beyond screen boundaries
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

void CPU::OP_Ex9E() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    if (keypad_buffer[*Vx]) {
        pc += 2;
    }
}

void CPU::OP_ExA1() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    if (!keypad_buffer[*Vx]) {
        pc += 2;
    }
}

void CPU::OP_Fx07() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    *Vx = delayTimer;
}

void CPU::OP_Fx0A() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    for (uint8_t i = 0;i < 16;i++) {
        if (keypad_buffer[i]) {
            *Vx = i;
        }
        else {
            pc -= 2;
        }
    }
}

void CPU::OP_Fx15() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    delayTimer = *Vx;
}

void CPU::OP_Fx18() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    soundTimer = *Vx;
}

void CPU::OP_Fx1E() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    I = I + *Vx;
}

void CPU::OP_Fx29() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    uint8_t digit = *Vx & 0xF;
    I = Presets::font_offset + (5U * digit);
}

void CPU::OP_Fx33() {
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    uint8_t value = *Vx;
    memory[I + 2] = value % 10;
    value /= 10;
    memory[I + 1] = value % 10;
    value /= 10;
    memory[I] = value % 10;
}

void CPU::OP_Fx55() {
    uint8_t V_offset = (opcode & 0x0F00U) >> 8;
    memcpy(memory + I, V, V_offset);
}

void CPU::OP_Fx65() {
    uint8_t V_offset = (opcode & 0x0F00U) >> 8;
    memcpy(V, memory + I, V_offset);
}