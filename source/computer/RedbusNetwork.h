#pragma once

#include <cstdint>
#include <set>

class RedbusDevice;

class RedbusNetwork
{
public:
	RedbusNetwork() = default;

	void registerDevice(RedbusDevice * device) { devices.insert(device); };
	void removeDevice(RedbusDevice * device) { devices.erase(device); };

	RedbusDevice * findDevice(uint8_t address);
private:
	std::set<RedbusDevice *> devices;
};
