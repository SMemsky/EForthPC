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
private:
	enum {
		Carry		= 1 << 0,
		Zero		= 1 << 1,
		Interrupt	= 1 << 2,
		Decimal		= 1 << 3,
		Breakpoint	= 1 << 4,
		Unused		= 1 << 5,
		Overflow	= 1 << 6,
		Sign		= 1 << 7
	};

	void loadBootImage();

	static unsigned const bankSize = 8 * 1024;
	static unsigned const maxBankCount = 8;
	static unsigned const memorySize = maxBankCount * bankSize;
	static unsigned const bootImageOffset;
	static unsigned const bootImageSize;

	static std::string const bootImagePath;

	std::array<uint8_t, memorySize> memory;
	unsigned memoryBanks;

	struct {
		uint16_t A;
		uint8_t X;
		uint8_t Y;
		uint16_t D;
		uint16_t SP;
		uint16_t PC;
		uint16_t R;
	} regs;

	uint16_t flags;

	uint16_t brkAddress;
	uint16_t porAddress;

	unsigned remainingCycles;
	bool isRunning;
};
