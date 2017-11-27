#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Floppy.h"
#include "RedbusConnectable.h"

class FloppyDrive : public RedbusConnectable
{
public:
	FloppyDrive(uint8_t address);

	void setDisk(Floppy floppy);
	Floppy const & getDisk() const;
	void ejectDisk();

	uint8_t read(uint8_t address) override;
	void write(uint8_t address, uint8_t value) override;
private:
	void readDiskNameCommand();
	void writeDiskNameCommand();
	void readDiskSerialCommand();
	void readDiskSectorCommand();
	void writeDiskSectorCommand();
	void executeCommand();

	std::array<uint8_t, 128> dataBuffer;

	Floppy disk;
	bool ejected;

	struct {
		uint8_t command;
		uint16_t sector;
	} regs;
};
