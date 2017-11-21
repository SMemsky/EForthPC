#include <iostream>
#include <string>
#include <vector>

#include "common/FileUtil.h"
#include "processor/Processor.h"

void printUsage(std::string const & program)
{
	std::cout << "Usage:\n"
		<< "    " << program << " <disk-image>" << std::endl;
}

int main(int argc, char * argv[])
{
	std::vector<std::string> const arguments(argv, argv + argc);
	if (arguments.size() < 2) {
		printUsage(arguments[0]);
		std::exit(1);
	}

	auto disk = loadFile(arguments[1]);
	std::cout << disk.size() << std::endl;

	return 0;
}
