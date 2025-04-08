#include "CHIP-8.h"
#include "IOLayer.h"
#include <SDL3/SDL.h>

bool halt = true;
int cycleDelay = 3; // Delay per cycle in ms
int videoScale = 10;
CHIP_8 chip8;

int main() {
	IOLayer ioLayer("CHIP-8 Emulator", 1000, 700, VIDEO_WIDTH, VIDEO_HEIGHT);
	
	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	Uint64 freq = SDL_GetPerformanceFrequency(); // ticks per second
	double lastCycleTime_ms = static_cast<double>(SDL_GetPerformanceCounter() * 1000) / freq;

	bool running = true;

	while (running) {
		// Handle ROM Loading
		if (ioLayer.loadROM) {
			ioLayer.loadROM = false;
			chip8.LoadROMFile(ioLayer.romPath.c_str());
			chip8.Reset();
		}

		// Handle SDL Events
		running = !ioLayer.HandleEvent(chip8.key);

		double currentTime_ms = static_cast<double>(SDL_GetPerformanceCounter() * 1000) / freq;
		float dt = currentTime_ms - lastCycleTime_ms;

		// Only execute cycle when a cycleTime has passed
		if ((dt > double(cycleDelay)) && !halt) {
			lastCycleTime_ms = currentTime_ms;

			// Execute a CPU cycle
			chip8.Cycle();
		}

		ioLayer.Render(chip8.video, videoPitch, &chip8.drawFlag, chip8.romPresent);
	}

	return 0;
}