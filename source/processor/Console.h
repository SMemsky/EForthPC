#pragma once

#include <array>
#include <cstdint>

#include "RedbusDevice.h"
#include "RedbusNetwork.h"

class Console : public RedbusDevice
{
public:
	Console(RedbusNetwork & network, uint8_t address);

	uint8_t read(uint8_t address) override;
	void write(uint8_t address, uint8_t value) override;

	void debugPrint() const;
private:
	static unsigned const screenWidth = 80;
	static unsigned const screenHeight = 50;
	static unsigned const kbBufferSize = 16;

	std::array<uint8_t, screenWidth*screenHeight> screen;
	std::array<uint8_t, kbBufferSize> kbBuffer;

	uint8_t memoryRow;
	uint8_t cursorX;
	uint8_t cursorY;
	uint8_t cursorMode;
	uint8_t kbStart;
	uint8_t kbPosition;
	uint8_t blitMode;
	uint8_t blitXS;
	uint8_t blitYS;
	uint8_t blitXD;
	uint8_t blitYD;
	uint8_t blitW;
	uint8_t blitH;
};
