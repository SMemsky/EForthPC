#pragma once

#include <string>
#include <vector>

class Floppy
{
public:
	Floppy() = default;

	Floppy(std::string name, std::vector<uint8_t> image) :
		name(std::move(name)),
		image(std::move(image))
	{}

	std::string const & getName() const { return name; };
	std::vector<uint8_t> const & getImage() const { return image; };
	std::vector<uint8_t> & getImage() { return image; };

	void setName(std::string name) { this->name = std::move(name); };
	void setImage(std::vector<uint8_t> image) { this->image = std::move(image); };
private:
	std::string name;
	std::vector<uint8_t> image;
};
