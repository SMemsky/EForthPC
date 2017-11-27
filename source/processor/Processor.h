#pragma once

#include <array>
#include <cstdint>

#include "RedbusDevice.h"
#include "RedbusNetwork.h"

class Processor : public RedbusDevice
{
public:
	Processor(RedbusNetwork & network, unsigned memoryBanks, uint8_t address);

	void coldBoot();
	void warmBoot();
	void halt();

	void runTick();

	uint8_t read(uint8_t address) override;
	void write(uint8_t address, uint8_t value) override;
private:
	enum Flag {
		Carry		= 1 << 0,
		Zero		= 1 << 1,
		Interrupt	= 1 << 2,
		Decimal		= 1 << 3,
		FlagX		= 1 << 4,
		FlagM		= 1 << 5,
		Overflow	= 1 << 6,
		Sign		= 1 << 7,
		FlagE		= 1 << 8
	};

	void setFlags(uint8_t mask);
	void resetFlags(uint8_t mask);
	void setFlag(Flag flag);
	void clearFlag(Flag flag);
	bool getFlag(Flag flag);

	uint8_t readOnlyMemory(uint16_t address);
	uint8_t readMemory(uint16_t address);
	void writeOnlyMemory(uint16_t address, uint8_t value);
	void writeMemory(uint16_t address, uint8_t value);
	uint8_t readByte();
	uint16_t readM();
	uint16_t readX();
	uint16_t readM(uint16_t address);
	void writeM(uint16_t address, uint16_t value);
	uint16_t readBS();
	uint16_t readW();
	uint16_t readW(uint16_t address);
	uint16_t readBW();
	uint16_t readBXW();

	void updateNZ();
	void updateNZ(uint16_t value);
	void updateNZX(uint16_t value);

	void i_brc(bool condition);
	void i_cmp(uint16_t x, uint16_t y);
	void i_inc(uint16_t address);
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
		uint16_t X;
		uint16_t Y;
		uint16_t D;
		uint16_t SP;
		uint16_t PC;
		uint16_t R;
		uint16_t I;
	} regs;

	struct {
		uint8_t redbusAddress;
		uint16_t redbusWindow;
		uint16_t externalWindow;
		bool redbusEnabled;
		bool enableExternalWindow;
	} mmu;

	uint16_t flags;

	uint16_t brkAddress;
	uint16_t porAddress;

	uint32_t ticks;
	unsigned remainingCycles;
	bool isRunning;
	bool rbTimeout;
	bool waiTimeout;

	RedbusDevice * rbCache;
};
