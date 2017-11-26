#include <iostream>
#include <string>
#include <vector>

#include "common/FileUtil.h"
#include "processor/Floppy.h"
#include "processor/FloppyDrive.h"
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

	Floppy disk(arguments[1], loadFile(arguments[1]));

	FloppyDrive drive(2);
	drive.setDisk(disk);

	drive.write(0x82, 0x1);

	std::cout << +drive.read(0x82) << std::endl;
	for (unsigned i = 0; i < 128; ++i) {
		std::cout << +drive.read(i) << " ";
	}
	std::cout << std::endl;

	// Processor processor(8);
	// processor.warmBoot();
	// processor.runTick();

	return 0;
}
