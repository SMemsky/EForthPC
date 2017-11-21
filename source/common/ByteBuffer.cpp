#include "ByteBuffer.h"

#include <cassert>
#include <stdexcept>

ByteBuffer::ByteBuffer(std::vector<uint8_t> data) :
	data(std::move(data)),
	position(0)
{}

void ByteBuffer::read(void * buffer, std::size_t count, bool reverse)
{
	if (count > bytesLeft()) {
		throw std::runtime_error(
			std::string("Buffer underflow of ")
			+ std::to_string(count - bytesLeft())
			+ " bytes");
	}

	if (reverse) {
		for (std::size_t i = 0; i < count; ++i) {
			reinterpret_cast<uint8_t *>(buffer)[i] = data[position + count - i - 1];
		}
	} else {
		for (std::size_t i = 0; i < count; ++i) {
			reinterpret_cast<uint8_t *>(buffer)[i] = data[position + i];
		}
	}

	position += count;
}
