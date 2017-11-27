#include <iostream>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "common/FileUtil.h"
#include "computer/Console.h"
#include "computer/Floppy.h"
#include "computer/FloppyDrive.h"
#include "computer/Processor.h"
#include "computer/RedbusNetwork.h"

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

	RedbusNetwork net;
	Console console(net, 1);

	Floppy bootDisk(arguments[1], loadFile(arguments[1]));
	FloppyDrive drive(net, 2);
	drive.setDisk(bootDisk);

	Processor processor(net, 8, 0);
	processor.warmBoot();

	static unsigned const scale = 3;

	sf::RenderWindow window(sf::VideoMode(scale*350, scale*230, 32), "EForthPC");
	auto view = window.getView();
	view.reset(sf::FloatRect(0, 0, 350, 230));
	window.setView(view);

	// window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(144);

	unsigned long const usPerTick = 50 * 1000;
	unsigned long tickTimer = 0;

	sf::Clock frameTimer;
	frameTimer.restart();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close(); break;
			default:
				break;
			}
		}

		sf::Time delta = frameTimer.restart();
		tickTimer += delta.asMicroseconds();

		while (tickTimer >= usPerTick) {
			tickTimer -= usPerTick;

			processor.runTick();
		}

		window.clear();
		console.draw(window);
		window.display();
	}

	return 0;
}
