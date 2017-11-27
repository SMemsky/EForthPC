#include "RedbusDevice.h"

RedbusDevice::RedbusDevice(RedbusNetwork & network, uint8_t address) :
	network(network),
	address(address)
{
	network.registerDevice(this);
}

RedbusDevice::~RedbusDevice()
{
	network.removeDevice(this);
}
