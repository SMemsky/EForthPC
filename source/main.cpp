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

namespace {

constexpr unsigned screenScale  = 3;
constexpr unsigned screenWidth  = 350;
constexpr unsigned screenHeight = 230;

constexpr bool     useVsync       = false;
constexpr unsigned framerateLimit = 144;

constexpr uint8_t consoleAddress     = 0x01;
constexpr uint8_t floppyDriveAddress = 0x02;
constexpr uint8_t processorAddress   = 0x00;

// Microseconds per time quanta.
// See computer/Processor.cpp for CPU Clock timings.
constexpr long usPerTick = 50 * 1000; // 50 ms, 20Hz

}

void printUsage(std::string const & program) {
	std::cout << "Usage:\n     " << program << " <disk-image>" << std::endl;
}

struct Context {
public:
	RedbusNetwork net;

	Console console;
	FloppyDrive drive;
	Processor processor;

	sf::RenderWindow window;

	Context(uint8_t consoleAdr, uint8_t driveAdr, uint8_t cpuAdr,
			uint8_t bankCount) :
		console(net, consoleAdr),
		drive(net, driveAdr),
		processor(net, bankCount, cpuAdr),
		window()
	{}
};

void mainLoop(Context & context) {
	unsigned long tickTimer = 0;
	unsigned long ticks     = 0;

	sf::Clock frameTimer;
	frameTimer.restart();

	context.processor.warmBoot();

	while (context.window.isOpen()) {
		sf::Event event;
		while (context.window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				context.window.close(); break;
			case sf::Event::TextEntered: {
				uint8_t code = event.text.unicode;
				if (code == 10) {
					code = 13;
				}
				if (code > 0 && code <= 127) {
					context.console.pushKey(code);
				}
				break;
			}
			default:
				break;
			}
		}

		sf::Time delta = frameTimer.restart();
		tickTimer += delta.asMicroseconds();

		while (tickTimer >= usPerTick) {
			tickTimer -= usPerTick;
			++ticks;

			context.processor.runTick();
		}

		context.window.clear();
		context.console.draw(context.window, ticks);
		context.window.display();
	}
}

int main(int argc, char * argv[]) {
	std::vector<std::string> const arguments(argv, argv + argc);
	if (arguments.size() < 2) {
		printUsage(arguments[0]);
		std::exit(1);
	}

	// Configure RedBus network
	Context context(consoleAddress, floppyDriveAddress, processorAddress, 8);

	// Load boot image into floppy drive
	Floppy bootDisk(arguments[1], loadFile(arguments[1]));
	context.drive.setDisk(bootDisk);

	// Warm boot the 65EL02
	// context.processor.warmBoot();

	// Create main window
	context.window.create(
		sf::VideoMode(screenWidth*screenScale, screenHeight*screenScale, 32),
		"EForthPC");

	// Make sure rendering is done in a 350x230 point space
	auto view = context.window.getView();
	view.reset(sf::FloatRect(0, 0, screenWidth, screenHeight));
	context.window.setView(view);

	if (useVsync) {
		context.window.setVerticalSyncEnabled(true);
	} else {
		context.window.setFramerateLimit(framerateLimit);
	}

	mainLoop(context);

	return 0;
}
