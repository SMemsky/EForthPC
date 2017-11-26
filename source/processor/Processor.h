#pragma once

#include <array>
#include <cstdint>

class Processor
{
public:
	Processor(unsigned memoryBanks = 1);

	void coldBoot();
	void warmBoot();
	void halt();

	void runTick();
private:
	enum Flag {
		Carry		= 1 << 0,
		Zero		= 1 << 1,
		Interrupt	= 1 << 2,
		Decimal		= 1 << 3,
		Breakpoint	= 1 << 4,
		FlagM		= 1 << 5,
		Overflow	= 1 << 6,
		Sign		= 1 << 7,
		FlagE		= 1 << 8,
		FlagX		= 1 << 9
	};

	void setFlags(uint8_t mask);
	void setFlag(Flag flag);
	void clearFlag(Flag flag);
	bool getFlag(Flag flag);

	uint8_t readOnlyMemory(uint16_t address);
	uint8_t readMemory(uint16_t address);
	uint8_t readByte();
	uint16_t readM();
	uint16_t readM(uint16_t address);
	uint16_t readBS();
	uint16_t readW(uint16_t address);
	uint16_t readBXW();

	void updateNZ();

	void i_or(uint16_t value);

	void processMMU(uint8_t opcode);
	void processInstruction();

	void loadBootImage();

	static unsigned const bankSize = 8 * 1024;
	static unsigned const maxBankCount = 8;
	static unsigned const memorySize = maxBankCount * bankSize;
	static unsigned const bootImageOffset;
	static unsigned const bootImageSize;
	static unsigned const cyclesPerTick;

	static std::string const bootImagePath;

	std::array<uint8_t, memorySize> memory;
	unsigned memoryBanks;

	struct {
		uint16_t A;
		uint8_t B;
		uint8_t X;
		uint8_t Y;
		uint16_t D;
		uint16_t SP;
		uint16_t PC;
		uint16_t R;
		uint16_t I;
	} regs;

	struct {
		uint8_t RBA;
		uint8_t RBB;
		uint8_t RBW;
		uint8_t ERB;
		bool enable;
	} mmu;

	uint16_t flags;

	uint16_t brkAddress;
	uint16_t porAddress;

	unsigned remainingCycles;
	bool isRunning;
};
