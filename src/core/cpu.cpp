#include "cpu.h"
#include "utils.h"

#include<cstdlib>
#include<cstdint>
#include<cstdio>
#include<cstring>
#include<iostream>
#include<random>

void CPU::Init_ISTable() {
    //Initializing the instruction table

    IStable[0x0] = &CPU::OP_IStable0;
    IStable[0x1] = &CPU::OP_1nnn;
    IStable[0x2] = &CPU::OP_2nnn;
    IStable[0x3] = &CPU::OP_3xkk;
    IStable[0x4] = &CPU::OP_4xkk;
    IStable[0x5] = &CPU::OP_5xy0;
    IStable[0x6] = &CPU::OP_6xkk;
    IStable[0x7] = &CPU::OP_7xkk;
    IStable[0x8] = &CPU::OP_IStable8;
    IStable[0x9] = &CPU::OP_9xy0;
    IStable[0xA] = &CPU::OP_Annn;
    IStable[0xB] = &CPU::OP_Bnnn;
    IStable[0xC] = &CPU::OP_Cxkk;
    IStable[0xD] = &CPU::OP_Dxyn;
    IStable[0xE] = &CPU::OP_IStableE;
    IStable[0xF] = &CPU::OP_IStableF;

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

    IStableE[0x1] = &CPU::OP_ExA1;
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
}

CPU::CPU(uint8_t* memory_, uint8_t* keypad_buffer_, uint32_t* video_buffer_, int& clear_flag_)
    : Memory{ memory_ }, Keypad_Buffer{ keypad_buffer_ }, Video_Buffer{ video_buffer_ }, Clear_Flag{ clear_flag_ } {
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    I = 0;
    sp = 0;
    pc = 0;
    delayTimer = 0;
    soundTimer = 0;
    opcode = 0;
}

void CPU::Init() {
    Init_ISTable();
    pc = 0x200;
    //prng
    std::random_device seed;
    RANDOM_GENERATOR = std::mt19937(seed());
    PRN_DISTRIBUTE = std::uniform_int_distribution<int>(0, 255);
}

void CPU::Cycle() {
    opcode = (Memory[pc] << 8) | (Memory[pc + 1]);
    //std::cerr <<"0x"<< std::hex << opcode <<" ";
    pc += 2;
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
    //std::cerr<<"invalid opcode\n";
}

void CPU::OP_00E0() {
    //std::cerr << "clear\n";
    //clear video_buffer
    Clear_Flag = 1;
}

void CPU::OP_00EE() {
    //std::cerr << "Return from subroutine\n";
    sp--;
    pc = stack[sp];
}

void CPU::OP_1nnn() {
    //std::cerr << "jump to location nnn\n";
    pc = opcode & 0x0FFFU;
}

void CPU::OP_2nnn() {
    //std::cerr << "call subroutine nnn\n";
    stack[sp++] = pc;
    pc = opcode & 0xFFFU;
}

void CPU::OP_3xkk() {
    //std::cerr << "skip instruction if vx = kk\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    if (*Vx == (opcode & 0xFFU)) {
        pc += 2;
    }
}
void CPU::OP_4xkk() {
    //std::cerr << "skip instruction if vx!=kk\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    if (*Vx != (opcode & 0xFFU)) {
        pc += 2;
    }
}
void CPU::OP_5xy0() {
    //std::cerr << "skip instruction if vx=vy\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    if (*Vx == *Vy) {
        pc += 2;
    }
}

void CPU::OP_6xkk() {
    //std::cerr << "set vx to kk\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    *Vx = (opcode & 0xFFU);
}

void CPU::OP_7xkk() {
    //std::cerr << "add kk to vx\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    *Vx += (opcode & 0xFFU);
}

void CPU::OP_8xy0() {
    //std::cerr << "store vy in vx\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx = *Vy;
}

void CPU::OP_8xy1() {
    //std::cerr << "vx = vx|vy\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx |= *Vy;
}

void CPU::OP_8xy2() {
    //std::cerr << "vx = vx&vy\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx &= *Vy;
}

void CPU::OP_8xy3() {
    //std::cerr << "vx = vx^vy\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    *Vx ^= *Vy;
}

void CPU::OP_8xy4() {
    //std::cerr << "carry sum\n";
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
    //std::cerr << "subtract borrow from vx\n";
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
    //std::cerr << "vx SHR 1\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    //uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    V[0xF] = *Vx & 1U;
    *Vx >>= 1;
}

void CPU::OP_8xy7() {
    //std::cerr << "subtract borrow from vy";
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
    //std::cerr << "vx SHL 1\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    //uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    V[0xF] = (*Vx & 0x80U) >> 7;
    *Vx <<= 1;
}

void CPU::OP_9xy0() {
    //std::cerr << "skip instruction if vx!=vy\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x0F0U) >> 4];
    if (*Vx != *Vy) {
        pc += 2;
    }
}

void CPU::OP_Annn() {
    //std::cerr << "set index register to nnn\n";
    I = (opcode & 0xFFFU);
}

void CPU::OP_Bnnn() {
    //std::cerr << "jump to v0 + nnn\n";
    pc = (opcode & 0x0FFFU) + V[0];
}

void CPU::OP_Cxkk() {
    //std::cerr << "set random byte&kk\n";
    uint8_t* Vx = &V[(opcode & 0xF00U) >> 8];
    *Vx = (opcode & 0x0FFU) & PRN_DISTRIBUTE(RANDOM_GENERATOR);
}

void CPU::OP_Dxyn() {
    //std::cerr << "Draw\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    uint8_t* Vy = &V[(opcode & 0x00F0U) >> 4];
    uint8_t xPos = *Vx;
    uint8_t yPos = *Vy;

    uint8_t length = opcode & 0x000F;
    char sprite[0xF];
    memcpy(sprite, Memory + I, length);
    V[0xF] = 0;

    for (int y = 0; y < length; y++) {
        for (int x = 0; x < 8; x++) {
            if (sprite[y] & (0x80 >> x)) {
                // Wrap coordinates, if going beyond screen boundaries
                uint32_t* screenPixel =
                    &(Video_Buffer[(((yPos + y) % VIDEO_BUFFER_HEIGHT) * VIDEO_BUFFER_WIDTH) + ((xPos + x) % VIDEO_BUFFER_WIDTH)]);
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
    //std::cerr << "skip instruction if key with value Vx is pressed\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    if (Keypad_Buffer[*Vx]) {
        pc += 2;
    }
}

void CPU::OP_ExA1() {
    //std::cerr << "skip instruction if key with value Vx is not pressed\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    if (!Keypad_Buffer[*Vx]) {
        pc += 2;
    }
}

void CPU::OP_Fx07() {
    //std::cerr << "set vx=delay timer\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    *Vx = delayTimer;
}

void CPU::OP_Fx0A() {
    //std::cerr << "wait for key press\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    bool wait = true;
    for (int i = 0;i < 16;i++) {
        if (Keypad_Buffer[i]) {
            *Vx = (uint8_t)i;
            wait = false;
            break;
        }
    }
    if (wait) {
        pc -= 2;
    }
}

void CPU::OP_Fx15() {
    //std::cerr << "set delay timer\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    delayTimer = *Vx;
}

void CPU::OP_Fx18() {
    //std::cerr << "set sound timer\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    soundTimer = *Vx;
}

void CPU::OP_Fx1E() {
    //std::cerr << "Increment index register by vx";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    I += *Vx;
}

void CPU::OP_Fx29() {
    //std::cerr << "set I to location of sprite\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    uint8_t digit = *Vx & 0xF;
    I = Presets::font_offset + (5U * digit);
}

void CPU::OP_Fx33() {
    //std::cerr << "Store BCD representation\n";
    uint8_t* Vx = &V[(opcode & 0x0F00U) >> 8];
    uint8_t value = *Vx;
    Memory[I + 2] = value % 10;
    value /= 10;
    Memory[I + 1] = value % 10;
    value /= 10;
    Memory[I] = value % 10;
}

void CPU::OP_Fx55() {
    //std::cerr << "store register in memory\n";
    uint8_t V_offset = (opcode & 0x0F00U) >> 8;
    memcpy(Memory + I, V, V_offset);
}

void CPU::OP_Fx65() {
    //std::cerr << "read memory into register\n";
    uint8_t V_offset = (opcode & 0x0F00U) >> 8;
    memcpy(V, Memory + I, V_offset);
}