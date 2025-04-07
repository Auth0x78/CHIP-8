#include "CHIP-8.h"
#include "LayerSDL.h"
#include <SDL3/SDL.h>

int main() {
	const int videoScale = 10;
	const int cycleDelay = 2.5; // Delay per cycle in ms
	char const* romFile = RESOURCES_PATH "Breakout.ch8";

	LayerSDL layer("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);
	
	CHIP_8 chip8;
	chip8.LoadROMFile(romFile);
	
	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	Uint64 freq = SDL_GetPerformanceFrequency(); // ticks per second
	double lastCycleTime_ms = static_cast<double>(SDL_GetPerformanceCounter() * 1000) / freq;

	bool running = true;

	while (running) {
		// Handle SDL Events
		running = !layer.HandleEvent(chip8.key);

		double currentTime_ms = static_cast<double>(SDL_GetPerformanceCounter() * 1000) / freq;
		float dt = currentTime_ms - lastCycleTime_ms;

		// Only execute cycle when a cycleTime has passed
		if (dt > cycleDelay) {
			lastCycleTime_ms = currentTime_ms;

			// Execute a CPU cycle
			chip8.Cycle();

			// Draw Window using SDL layer
			layer.Update(chip8.video, videoPitch);
		}
	}

	return 0;
}