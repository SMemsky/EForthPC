#include "FileUtil.h"

#include <fstream>
#include <stdexcept>

std::vector<uint8_t> loadFile(std::string const & filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file) {
		throw std::runtime_error(
			std::string("Unable to open file '") + filename + "'");
	}

	std::vector<uint8_t> data(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char *>(data.data()), data.size());

	return data;
}
