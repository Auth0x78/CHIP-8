#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <functional>
#include <IOLayer.h>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int STACK_SIZE = 16;
const unsigned int DISPLAY_COLOR = 0xFFFFFFFF; // Default display color (R8_G8_B8_A8)

class CHIP_8 {
public:
	friend class IOLayer;

	CHIP_8();

	void LoadROMFile(const char* file);
	void Cycle();
	void Reset();

	uint8_t key[KEY_COUNT]; // 16 keys (0x0 to 0xF)
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]; // 64x32 pixel display
	bool drawFlag = false; // Flag to indicate if the display needs to be redrawn
	bool romPresent = false;

private:
	// Table functions
	void Table0();
	void Table8();
	void TableE();
	void TableF();

	void null(); 
	void CLS(); // OPCODE: 0x00E0
	void RET(); // OPCODE: 0x00EE
	
	void JMP(); // OPCODE: 0x1NNN
	void CALL(); // OPCODE: 0x2NNN

	void SE(); // OPCODE: 0x3XNN
	void SNE(); // OPCODE: 0x4XNN
	
	void SE_VX(); // OPCODE: 0x5XY0
	void LD(); // OPCODE: 0x6XNN
	void ADD(); // OPCODE: 0x7XNN
	void LD_VX(); // OPCODE: 0x8XY0
	void OR(); // OPCODE: 0x8XY1
	void AND(); // OPCODE: 0x8XY2
	void XOR(); // OPCODE: 0x8XY3
	void ADD_VX(); // OPCODE: 0x8XY4
	void SUB(); // OPCODE: 0x8XY5
	void SHR(); // OPCODE: 0x8XY6
	void SUBN(); // OPCODE: 0x8XY7
	void SHL(); // OPCODE: 0x8XYE
	void SNE_VX(); // OPCODE: 0x9XY0
	void LD_I(); // OPCODE: 0xANNN
	void JP_V0(); // OPCODE: 0xBNNN
	void RND(); // OPCODE: 0xCXNN
	void DRW(); // OPCODE: 0xDXYN
	void SKP(); // OPCODE: 0xEX9E
	void SKNP(); // OPCODE: 0xEXA1
	void LD_VX_DT(); // OPCODE: 0xFX07
	void LD_VX_K(); // OPCODE: 0xFX0A
	void LD_DT_VX(); // OPCODE: 0xFX15
	void LD_ST_VX(); // OPCODE: 0xFX18
	void ADD_I_VX(); // OPCODE: 0xFX1E
	void LD_F_VX(); // OPCODE: 0xFX29
	void LD_B_VX(); // OPCODE: 0xFX33
	void LD_I_VX(); // OPCODE: 0xFX55
	void LD_VX_I(); // OPCODE: 0xFX65

	uint8_t V[16]{}; // 16 registers (V0 to VF)
	// VF Register is special as it holds flags information
	uint16_t I{}; // Index register
	uint16_t PC{}; // Program counter, default to 0x200
	uint8_t SP{}; // Stack pointer, points to the current stack level, default 0

	// 4KBytes Memory
	uint8_t memory[MEMORY_SIZE]{}; // 4KB of memory

	// 16 level stack
	uint16_t stack[STACK_SIZE]{}; // 16 levels of stack

	uint8_t delayTimer{}; // Delay timer
	uint8_t soundTimer{}; // Sound timer

	uint16_t opcode{}; // Current opcode

private:
	std::default_random_engine randGen;
	std::uniform_int_distribution<int> dist;

	typedef void (CHIP_8::*Chip8Func)();
	Chip8Func table[0xF + 1];
	Chip8Func table0[0xE + 1];
	Chip8Func table8[0xE + 1];
	Chip8Func tableE[0xE + 1];
	Chip8Func tableF[0x65 + 1];
};