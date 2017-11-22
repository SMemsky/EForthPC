#include "Processor.h"

#include <cassert>
#include <iostream>

#include "common/FileUtil.h"

std::string const Processor::bootImagePath = "resources/rpcboot.bin";
unsigned const Processor::bootImageOffset = 1024;
unsigned const Processor::bootImageSize = 256;

Processor::Processor(unsigned memoryBanks) :
	memory(),
	memoryBanks(memoryBanks),
	regs{0, 0, 0, 0, 0, 0, 0},
	flags(0),
	brkAddress(8192),
	porAddress(8192),
	remainingCycles(0),
	isRunning(false)
{
	assert(memoryBanks != 0);
	assert(memoryBanks <= maxBankCount);

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

	loadBootImage();

	remainingCycles = 0;
	isRunning = false;
}

void Processor::warmBoot()
{
	regs.SP = 512;
	regs.R = 768;
	regs.PC = porAddress;
}

void Processor::halt()
{
	isRunning = false;
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
