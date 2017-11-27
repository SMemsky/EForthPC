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

void FloppyDrive::writeDiskNameCommand()
{
	std::string diskName = "";
	diskName.reserve(dataBuffer.size() - 1);

	for (unsigned i = 0; i < dataBuffer.size(); ++i) {
		if (dataBuffer[i] == 0) {
			break;
		}

		diskName += static_cast<char>(dataBuffer[i]);
	}

	disk.setName(diskName);

	regs.command = 0;
}

void FloppyDrive::readDiskSerialCommand()
{
	dataBuffer.fill(0);

	// For now...
	std::string const diskSerial = "deadbeef";
	std::copy(diskSerial.begin(), diskSerial.end(), dataBuffer.begin());

	regs.command = 0;
}

void FloppyDrive::readDiskSectorCommand()
{
	if (regs.sector > 2048) {
		regs.command = uint8_t(-1);
		return;
	}

	unsigned const sectorStart = regs.sector * 128;
	auto const & image = disk.getImage();

	if (image.size() < sectorStart + 128) {
		regs.command = uint8_t(-1);
		return;
	}

	for (unsigned i = 0; i < dataBuffer.size(); ++i) {
		dataBuffer[i] = image[sectorStart + i];
	}

	regs.command = 0;
}

void FloppyDrive::writeDiskSectorCommand()
{
	if (regs.sector > 2048) {
		regs.command = uint8_t(-1);
		return;
	}

	unsigned const sectorStart = regs.sector * 128;
	auto & image = disk.getImage();

	if (image.size() < sectorStart + 128) {
		image.resize(sectorStart + 128);
	}

	for (unsigned i = 0; i < dataBuffer.size(); ++i) {
		image[sectorStart + i] = dataBuffer[i];
	}

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
		writeDiskNameCommand(); break;
	case 3:
		readDiskSerialCommand(); break;
	case 4:
		readDiskSectorCommand(); break;
	case 5:
		writeDiskSectorCommand(); break;
	default:
		regs.command = uint8_t(-1);
	}
}
