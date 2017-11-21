#pragma once

#include <cstdint>
#include <string>
#include <vector>

class ByteBuffer
{
public:
	ByteBuffer() = default;
	ByteBuffer(std::vector<uint8_t> data);

	void read(void * buffer, std::size_t count, bool reverse = false);

	std::size_t bytesLeft() const { return data.size() - position; };
private:
	std::vector<uint8_t> const data;

	std::size_t position;
};
