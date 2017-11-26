#include "FloppyDrive.h"

FloppyDrive::FloppyDrive(uint8_t address) :
	RedbusConnectable(address),
	dataBuffer(),
	disk(),
	ejected(true),
	regs{0, 0}
{}

void FloppyDrive::setDisk(Floppy floppy)
{
	disk = std::move(floppy);
	ejected = false;
}

Floppy const & FloppyDrive::getDisk() const
{
	return disk;
}

void FloppyDrive::ejectDisk()
{
	ejected = true;
}

uint8_t FloppyDrive::read(uint8_t address)
{
	if (address < 128) {
		return dataBuffer[address];
	}

	switch (address) {
	case 0x80:
		return regs.sector & 0xff;
	case 0x81:
		return regs.sector >> 8;
	case 0x82:
		return regs.command;
	default:
		return 0;
	}
}

void FloppyDrive::write(uint8_t address, uint8_t value)
{
	if (address < 128) {
		dataBuffer[address] = value;
		return;
	}

	switch (address) {
	case 0x80:
		regs.sector = (regs.sector & 0xff00) | value;
		break;
	case 0x81:
		regs.sector = (regs.sector & 0xff) | (value << 8);
		break;
	case 0x82:
		regs.command = value;
		executeCommand(); // For now, execute immediately
		break;
	default:
		break;
	}
}

void FloppyDrive::readDiskNameCommand()
{
	dataBuffer.fill(0);

	std::string const diskName = disk.getName().substr(0, dataBuffer.size());
	std::copy(diskName.begin(), diskName.end(), dataBuffer.begin());

	regs.command = 0;
}

void FloppyDrive::executeCommand()
{
	if (ejected) {
		regs.command = uint8_t(-1);
		return;
	}

	switch (regs.command) {
	case 0:
		return;
	case 1:
		readDiskNameCommand(); break;
	case 2:
	case 3:
	case 4:
	case 5:
	default:
		regs.command = uint8_t(-1);
	}
}
