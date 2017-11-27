#pragma once

#include <cstdint>

#include "RedbusNetwork.h"

class RedbusDevice
{
public:
	RedbusDevice(RedbusNetwork & network, uint8_t address);
	~RedbusDevice();

	uint8_t getAddress() const { return address; };
	void setAddress(uint8_t address) { this->address = address; };

	RedbusDevice * findDevice(uint8_t address) { return network.findDevice(address); };

	virtual uint8_t read(uint8_t address) = 0;
	virtual void write(uint8_t address, uint8_t value) = 0;
private:
	RedbusNetwork & network;

	uint8_t address;
};
