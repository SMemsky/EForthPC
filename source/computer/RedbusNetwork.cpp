#include "RedbusNetwork.h"

#include "RedbusDevice.h"

RedbusDevice * RedbusNetwork::findDevice(uint8_t address)
{
	auto const end = devices.end();
	for (auto it = devices.begin(); it != end; ++it) {
		if ((*it)->getAddress() == address) {
			return *it;
		}
	}

	return nullptr;
}
