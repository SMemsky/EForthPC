#include "Processor.h"

#include <cassert>
#include <iostream>

#include "common/FileUtil.h"

std::string const Processor::bootImagePath = "resources/rpcboot.bin";
unsigned const Processor::bootImageOffset = 1024;
unsigned const Processor::bootImageSize = 256;
unsigned const Processor::cyclesPerTick = 1000;

Processor::Processor(unsigned memoryBanks) :
	memory(),
	memoryBanks(memoryBanks),
	regs{0, 0, 0, 0, 0, 0, 0, 0, 0},
	mmu{0, 0, 0, 0, false},
	flags(0),
	brkAddress(8192),
	porAddress(8192),
	remainingCycles(0),
	isRunning(false)
{
	assert(this->memoryBanks != 0);
	assert(this->memoryBanks <= maxBankCount);

	coldBoot();
}

void Processor::coldBoot()
{
	brkAddress = porAddress = 8192;
	regs.SP = 512;
	regs.PC = 1024;
	regs.R = 768;

	regs.A = regs.X = regs.Y = regs.D = 0;
	flags = 0;
	setFlag(FlagE);
	setFlag(FlagM);
	setFlag(FlagX);

	std::cout << "Cold boot flags: " << flags << std::endl;

	memory[0] = 2; // Disk
	memory[1] = 1; // Console

	loadBootImage();

	remainingCycles = 0;
	isRunning = false;
}

void Processor::warmBoot()
{
	if (isRunning) {
		regs.SP = 512;
		regs.R = 768;
		regs.PC = porAddress;
	}

	remainingCycles = 0;
	isRunning = true;
}

void Processor::halt()
{
	isRunning = false;
}

void Processor::runTick()
{
	if (!isRunning) {
		return;
	}

	remainingCycles += cyclesPerTick;
	if (remainingCycles > 100 * cyclesPerTick) {
		remainingCycles = 100 * cyclesPerTick;
	}

	while (isRunning &&
		remainingCycles-- > 0)
	{
		processInstruction();
	}
}

void Processor::setFlags(uint8_t mask)
{
	bool flagM = getFlag(FlagM);
	uint8_t baseFlags = flags & 0xff;
	baseFlags &= ~mask;
	flags = baseFlags | (flags & 0xff00);

	if (getFlag(FlagE)) {
		clearFlag(Breakpoint);
		clearFlag(FlagM);
	} else {
		if (getFlag(FlagX)) {
			regs.X &= 0xff;
			regs.Y &= 0xff;
		}
		if (getFlag(FlagM) != flagM) {
			if (getFlag(FlagM)) {
				regs.B = regs.A >> 8;
				regs.A &= 0xff;
			} else {
				regs.A |= regs.B << 8;
			}
		}
	}
}

void Processor::setFlag(Flag flag)
{
	flags |= flag;
}

void Processor::clearFlag(Flag flag)
{
	flags &= ~flag;
}

bool Processor::getFlag(Flag flag)
{
	return flags & flag;
}

uint8_t Processor::readOnlyMemory(uint16_t address)
{
	if ((address >> 13) + 1u > memoryBanks) {
		return 255;
	}

	return memory[address];
}

uint8_t Processor::readMemory(uint16_t address)
{
	return readOnlyMemory(address);
}

uint8_t Processor::readByte()
{
	return readMemory(regs.PC++);
}

uint16_t Processor::readM()
{
	uint16_t i = readMemory(regs.PC++);
	if (!getFlag(FlagM)) {
		i |= readMemory(regs.PC++) << 8;
	}
	return i;
}

uint16_t Processor::readM(uint16_t address)
{
	uint16_t i = readMemory(address);
	if (!getFlag(FlagM)) {
		i |= readMemory(address+1) << 8;
	}
	return i;
}

uint16_t Processor::readBS()
{
	return readMemory(regs.PC++) + regs.SP;
}

uint16_t Processor::readW(uint16_t address)
{
	uint16_t i = readMemory(address);
	i |= readMemory(address + 1) << 8;
	return i;
}

uint16_t Processor::readBXW()
{
	uint16_t i = (readMemory(regs.PC++) + regs.X) & 0xff;
	uint16_t j = readMemory(i);
	j |= readMemory(i + 1) << 8;
	return j;
}

void Processor::updateNZ()
{
	if (regs.A & (getFlag(FlagM) ? 128 : 32768)) {
		setFlag(Sign);
	} else {
		clearFlag(Sign);
	}

	if (regs.A == 0) {
		setFlag(Zero);
	} else {
		clearFlag(Zero);
	}
}

void Processor::i_or(uint16_t value)
{
	regs.A |= value;
	updateNZ();
}

void Processor::processMMU(uint8_t opcode)
{
	std::cout << std::hex << "Got MMU opcode: " << +opcode << std::dec << std::endl;

	switch (opcode) {
	case 0x00:
		if (mmu.RBA != (regs.A & 0xff)) {
			// todo
			mmu.RBA = (regs.A & 0xff);
		}
		std::cout << "Redbus window mapped to device " << +mmu.RBA << std::endl;
		break;
	case 0x01:
		mmu.RBB = regs.A;
		std::cout << "Redbus window set to " << +mmu.RBB << std::endl;
		break;
	case 0x02:
		mmu.enable = true;
		std::cout << "Redbus enabled" << std::endl;
		break;
	default:
		std::cout << "Unknown MMU opcode: " << std::hex << +opcode << std::dec << std::endl;
		isRunning = false;
		break;
	}
}

void Processor::processInstruction()
{
	uint8_t opcode = readMemory(regs.PC++);
	std::cout << std::hex << "Got opcode: " << +opcode << std::dec << std::endl;

	switch (opcode) {
	case 0x01:
		i_or(readM(readBXW()));
		break;
	case 0x02:
		regs.PC = readW(regs.I);
		regs.I += 2;
		break;
	case 0x03:
		i_or(readM(readBS()));
		break;
	case 0x18:
		clearFlag(Carry); break;
	// case 0x64:
	// 	break;
	case 0xa5:
		regs.A = readM(readByte());
		updateNZ();
		break;
	case 0xa9:
		regs.A = readM();
		updateNZ();
		break;
	case 0xc2:
		setFlags(readByte());
		break;
	case 0xef:
		processMMU(readByte());
		break;
	case 0xfb:
		if (getFlag(FlagE) == getFlag(Carry)) {
			break;
		}

		if (getFlag(FlagE)) {
			clearFlag(FlagE);
			setFlag(Carry);
		} else {
			setFlag(FlagE);
			clearFlag(Carry);
			if (!getFlag(FlagM)) {
				regs.B = regs.A >> 8;
			}
			setFlag(FlagM);
			setFlag(FlagX);
			regs.A &= 0xff;
			regs.Y &= 0xff;
			regs.X &= 0xff;
		}

		break;
	default:
		std::cout << "Unknown opcode: " << std::hex << +opcode << " at " << regs.PC-1 << std::dec << std::endl;
		isRunning = false;
		break;
	}
}

void Processor::loadBootImage()
{
	try {
		auto bootImage = loadFile(bootImagePath);
		for (unsigned i = 0; i < bootImageSize && i < bootImage.size(); ++i) {
			memory[bootImageOffset + i] = bootImage[i];
		}
	} catch (std::runtime_error & e) {
		std::cout << e.what() << std::endl;
	}
}
