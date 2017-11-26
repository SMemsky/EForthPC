#pragma once

#include <cstdint>

class RedbusConnectable
{
public:
	RedbusConnectable(uint8_t address);
	~RedbusConnectable() = default;

	virtual uint8_t getAddress() const { return address; };
	virtual void setAddress(uint8_t address) { this->address = address; };

	virtual uint8_t read(uint8_t address) = 0;
	virtual void write(uint8_t address, uint8_t value) = 0;
private:
	uint8_t address;
};
