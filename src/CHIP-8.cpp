#include "CHIP-8.h"

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;

const unsigned int FONTSET_SIZE = 80;

uint8_t fontSet[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void CHIP_8::Table0() {
	((*this).*(table0[opcode & 0x000Fu]))();
}

void CHIP_8::Table8() {
	((*this).*(table8[opcode & 0x000Fu]))();
}

void CHIP_8::TableE() {
	((*this).*(tableE[opcode & 0x000Fu]))();
}

void CHIP_8::TableF() {
	((*this).*(tableF[opcode & 0x00FFu]))();
}

CHIP_8::CHIP_8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
	PC = START_ADDRESS;
	SP = 0;
	memset(video, 0, sizeof(video)); // Clear the display
	memset(key, 0, sizeof(key));
	memset(memory, 0, sizeof(memory));
	memset(stack, 0, sizeof(stack));

	// Load fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontSet[i];
	}

	// Initialize RNG
	dist = std::uniform_int_distribution<int>(0, 255U);

	// Initialize function pointer table
	table[0x0] = &CHIP_8::Table0;
	table[0x1] = &CHIP_8::JMP;
	table[0x2] = &CHIP_8::CALL;
	table[0x3] = &CHIP_8::SE;
	table[0x4] = &CHIP_8::SNE;
	table[0x5] = &CHIP_8::SE_VX;
	table[0x6] = &CHIP_8::LD;
	table[0x7] = &CHIP_8::ADD;
	table[0x8] = &CHIP_8::Table8;
	table[0x9] = &CHIP_8::SNE_VX;
	table[0xA] = &CHIP_8::LD_I;
	table[0xB] = &CHIP_8::JP_V0;
	table[0xC] = &CHIP_8::RND;
	table[0xD] = &CHIP_8::DRW;
	table[0xE] = &CHIP_8::TableE;
	table[0xF] = &CHIP_8::TableF;

	for (size_t i = 0; i <= 0xE; ++i) {
		table0[i] = &CHIP_8::null;
		table8[i] = &CHIP_8::null;
		tableE[i] = &CHIP_8::null;
	}

	table0[0x0] = &CHIP_8::CLS;
	table0[0xE] = &CHIP_8::RET;

	table8[0x0] = &CHIP_8::LD_VX;
	table8[0x1] = &CHIP_8::OR;
	table8[0x2] = &CHIP_8::AND;
	table8[0x3] = &CHIP_8::XOR;
	table8[0x4] = &CHIP_8::ADD_VX;
	table8[0x5] = &CHIP_8::SUB;
	table8[0x6] = &CHIP_8::SHR;
	table8[0x7] = &CHIP_8::SUBN;
	table8[0xE] = &CHIP_8::SHL;

	tableE[0x1] = &CHIP_8::SKNP;
	tableE[0xE] = &CHIP_8::SKP;

	for (size_t i = 0; i <= 0x65; ++i) {
		tableF[i] = &CHIP_8::null;
	}

	tableF[0x07] = &CHIP_8::LD_VX_DT;
	tableF[0x0A] = &CHIP_8::LD_VX_K;
	tableF[0x15] = &CHIP_8::LD_DT_VX;
	tableF[0x18] = &CHIP_8::LD_ST_VX;
	tableF[0x1E] = &CHIP_8::ADD_I_VX;
	tableF[0x29] = &CHIP_8::LD_F_VX;
	tableF[0x33] = &CHIP_8::LD_B_VX;
	tableF[0x55] = &CHIP_8::LD_I_VX;
	tableF[0x65] = &CHIP_8::LD_VX_I;
}

void CHIP_8::LoadROMFile(const char* file) {
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream ROM(file, std::ios::binary | std::ios::ate);

	if (ROM.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = ROM.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		ROM.seekg(0, std::ios::beg);
		ROM.read(buffer, size);
		ROM.close();

		// Copy the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i) {
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}

	romPresent = true;
}

void CHIP_8::Cycle(){
	// Fetch
	opcode = (memory[PC] << 8u) | memory[PC + 1];

	// Increment the PC before we execute anything
	PC += 2;

	// Decode and Execute
	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

	// Decrement the delay timer if it's been set
	if (delayTimer > 0)
		--delayTimer;

	// Decrement the sound timer if it's been set
	if (soundTimer > 0)
		--soundTimer;
}

void CHIP_8::Reset() {
	PC = START_ADDRESS;
	SP = 0;
	memset(video, 0, sizeof(video)); // Clear the display
	memset(key, 0, sizeof(key));
	memset(stack, 0, sizeof(stack));
}

void CHIP_8::null() {
}

void CHIP_8::CLS() {
	memset(video, 0, sizeof(video)); // Clear the display
}

void CHIP_8::RET() {
	--SP;
	PC = stack[SP];
}

void CHIP_8::JMP() {
	// OPCODE: 1nnn
	uint16_t address = opcode & 0x0fffu;

	PC = address;
}

void CHIP_8::CALL() {
	// OPCODE: 2nnn
	uint16_t address = opcode & 0x0FFFu;
	stack[SP] = PC;
	++SP;

	PC = address;
}

void CHIP_8::SE() {
	uint8_t x = (opcode & 0x0F00u) >> 8u; // Get the register number
	uint8_t value = opcode & 0x0FFu;

	if(V[x] == value) {
		PC += 2; // Skip the next instruction
	}
}

void CHIP_8::SNE() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (V[x] != byte) {
		PC += 2;
	}
}

void CHIP_8::SE_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	if (V[x] == V[y]) {
		PC += 2;
	}
}

void CHIP_8::LD() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	V[x] = byte;
}

void CHIP_8::ADD() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	V[x] += byte;
}

void CHIP_8::LD_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	V[x] = V[y];
}

void CHIP_8::OR() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	V[x] |= V[y];
}

void CHIP_8::AND() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	V[x] &= V[y];
}

void CHIP_8::XOR() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	V[x] ^= V[y];
}

void CHIP_8::ADD_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = V[x] + V[y];
	V[0xF] = (sum > 0xFFu) ? 1 : 0; // Set carry flag
	V[x] = sum & 0xFF; // Store the result in V[x]
}

void CHIP_8::SUB() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	V[0xF] = (V[x] > V[y]) ? 1 : 0; // Set carry flag
	V[x] -= V[y];
}

void CHIP_8::SHR() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	V[0xF] = V[x] & 0x1u; // Store the least significant bit in VF
	V[x] >>= 1; // Shift right
}

void CHIP_8::SUBN() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	V[0xF] = (V[y] > V[x]) ? 1 : 0; // Set carry flag
	V[x] = V[y] - V[x];
}

void CHIP_8::SHL() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	V[0xF] = (V[x] & 0x80u) ? 1 : 0; // Store the most significant bit in VF
	V[x] <<= 1; // Shift left
}

void CHIP_8::SNE_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;

	if (V[x] != V[y]) {
		PC += 2;
	}
}

void CHIP_8::LD_I() {
	uint16_t address = opcode & 0x0FFFu;

	I = address;
}

void CHIP_8::JP_V0() {
	uint16_t address = opcode & 0x0FFFu;

	PC = V[0] + address;
}

void CHIP_8::RND() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	V[x] = static_cast<uint8_t>(dist(randGen)) & byte; // Generate a random number and mask it with the byte
}

void CHIP_8::DRW() {
	drawFlag = true;
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = V[x];
	uint8_t yPos = V[y];

	V[0xF] = 0; // Reset the collision flag

	for (uint8_t row = 0; row < height; ++row) {
		uint8_t sprite = memory[I + row];
		for (uint8_t col = 0; col < 8; ++col) {
			if ((sprite & (0x80 >> col)) != 0) {
				uint16_t x = (xPos + col) % VIDEO_WIDTH;
				uint16_t y = (yPos + row) % VIDEO_HEIGHT;
				uint32_t index = x + (y * VIDEO_WIDTH);

				if (video[index] != 0) {
					V[0xF] = 1; // Set collision flag
				}
				video[index] ^= DISPLAY_COLOR; // Toggle 32-bit pixel
			}
		}
	}
}

void CHIP_8::SKP() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	if (key[V[x]] != 0) {
		PC += 2; // Skip the next instruction
	}
}

void CHIP_8::SKNP() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	if (key[V[x]] == 0) {
		PC += 2; // Skip the next instruction
	}
}

void CHIP_8::LD_VX_DT() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	V[x] = delayTimer;
}

void CHIP_8::LD_VX_K() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	if (key[0])
	{
		V[x] = 0;
	}
	else if (key[1])
	{
		V[x] = 1;
	}
	else if (key[2])
	{
		V[x] = 2;
	}
	else if (key[3])
	{
		V[x] = 3;
	}
	else if (key[4])
	{
		V[x] = 4;
	}
	else if (key[5])
	{
		V[x] = 5;
	}
	else if (key[6])
	{
		V[x] = 6;
	}
	else if (key[7])
	{
		V[x] = 7;
	}
	else if (key[8])
	{
		V[x] = 8;
	}
	else if (key[9])
	{
		V[x] = 9;
	}
	else if (key[10])
	{
		V[x] = 10;
	}
	else if (key[11])
	{
		V[x] = 11;
	}
	else if (key[12])
	{
		V[x] = 12;
	}
	else if (key[13])
	{
		V[x] = 13;
	}
	else if (key[14])
	{
		V[x] = 14;
	}
	else if (key[15])
	{
		V[x] = 15;
	}
	else
	{
		PC -= 2;
	}
}

void CHIP_8::LD_DT_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	delayTimer = V[x];
}

void CHIP_8::LD_ST_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	soundTimer = V[x];
}

void CHIP_8::ADD_I_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	I += V[x];
}

void CHIP_8::LD_F_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	I = FONTSET_START_ADDRESS + (V[x] * 5); // Set I to the address of the font
}

void CHIP_8::LD_B_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t value = V[x];

	// Ones-place
	memory[I + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[I + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[I] = value % 10;
}

void CHIP_8::LD_I_VX() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= x; ++i) {
		memory[I + i] = V[i];
	}
}

void CHIP_8::LD_VX_I() {
	uint8_t x = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= x; ++i) {
		V[i] = memory[I + i];
	}
}