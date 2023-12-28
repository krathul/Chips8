#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <iostream>
#include <random>

#define VIDEO_BUFFER_WIDTH 64
#define VIDEO_BUFFER_HEIGHT 32

class CPU {
private:
	//prng
	std::mt19937 RANDOM_GENERATOR;
	std::uniform_int_distribution<int> PRN_DISTRIBUTE;

	uint8_t* memory;
	uint8_t V[16];
	uint16_t I;
	uint8_t sp;
	uint16_t pc;
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint16_t opcode;
	uint16_t stack[16];

	//Instruction Table
	void (CPU::* IStable[0xF + 1])(void);
	void (CPU::* IStable0[0xE + 1])(void);
	void (CPU::* IStable8[0xE + 1])(void);
	void (CPU::* IStableE[0xE + 1])(void);
	void (CPU::* IStableF[0x65 + 1])(void);

	//Instuctions
	void OP_NULL();
	void OP_IStable0();
	void OP_IStable8();
	void OP_IStableE();
	void OP_IStableF();

	void OP_00E0();
	void OP_00EE();
	//
	void OP_1nnn();
	void OP_2nnn();
	void OP_3xkk();
	void OP_4xkk();
	void OP_5xy0();
	void OP_6xkk();
	void OP_7xkk();
	//
	void OP_8xy0();
	void OP_8xy1();
	void OP_8xy2();
	void OP_8xy3();
	void OP_8xy4();
	void OP_8xy5();
	void OP_8xy6();
	void OP_8xy7();
	void OP_8xyE();
	//
	void OP_9xy0();
	void OP_Annn();
	void OP_Bnnn();
	void OP_Cxkk();
	void OP_Dxyn();
	//
	void OP_Ex9E();
	void OP_ExA1();
	//
	void OP_Fx07();
	void OP_Fx0A();
	void OP_Fx15();
	void OP_Fx18();
	void OP_Fx1E();
	void OP_Fx29();
	void OP_Fx33();
	void OP_Fx55();
	void OP_Fx65();

public:
	uint8_t keypad_buffer[16];
	uint32_t video_buffer[64 * 32];
	int drawFlag;

	CPU();
	void load(char* buffer, std::size_t buffer_size);
	void Cycle();
};

#endif


