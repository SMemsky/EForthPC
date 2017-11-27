#include "Console.h"

#include <iostream>

Console::Console(RedbusNetwork & network, uint8_t address) :
	RedbusDevice(network, address),
	screen(),
	kbBuffer(),
	memoryRow(),
	cursorX(),
	cursorY(),
	cursorMode(2),
	kbStart(),
	kbPosition(),
	blitMode(),
	blitXS(),
	blitYS(),
	blitXD(),
	blitYD(),
	blitW(),
	blitH()
{
	screen.fill(32);
}

uint8_t Console::read(uint8_t address)
{
	std::cout << "Read console: " << +address << std::endl;

	if (address >= 16
		&& address < 96)
	{
		return screen[memoryRow * 80 + address - 16];
	}

	switch (address) {
	case 0: return memoryRow;
	case 1: return cursorX;
	case 2: return cursorY;
	case 3: return cursorMode;
	case 4: return kbStart;
	case 5: return kbPosition;
	case 6: return kbBuffer[kbStart];
	case 7: return blitMode;
	case 8: return blitXS;
	case 9: return blitYS;
	case 10: return blitXD;
	case 11: return blitYD;
	case 12: return blitW;
	case 13: return blitH;
	default: return 0;
	}
}

void Console::write(uint8_t address, uint8_t value)
{
	if (address >= 16
		&& address < 96)
	{
		screen[memoryRow * 80 + address - 16] = value; return;
	}

	switch (address) {
	case 0: memoryRow = value; if (memoryRow > 49) memoryRow = 49; return;
	case 1: cursorX = value; return;
	case 2: cursorY = value; return;
	case 3: cursorMode = value; return;
	case 4: kbStart = value & 0xf; return;
	case 5: kbPosition = value & 0xf; return;
	case 6: kbBuffer[kbStart] = value; return;
	case 7: blitMode = value; return;
	case 8: blitXS = value; return;
	case 9: blitYS = value; return;
	case 10: blitXD = value; return;
	case 11: blitYD = value; return;
	case 12: blitW = value; return;
	case 13: blitH = value; return;
	default: return;
	}
}

void Console::debugPrint() const
{
	for (unsigned y = 0; y < screenHeight; ++y) {
		for (unsigned x = 0; x < screenWidth; ++x) {
			uint8_t symbol = screen[y * screenWidth + x];
			std::cout << static_cast<char>(symbol);
		}
		std::cout << std::endl;
	}
}
