#pragma once
#include <iostream>
#include <unordered_map>
#include <SDL3/SDL.h>

class LayerSDL {
public:
	LayerSDL(char const* title, int width, int height, int textureWidth, int textureHeight);
	void Update(void* buffer, int pitch);
	bool HandleEvent(uint8_t* pKeys);

	~LayerSDL();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* mainTexture;

	std::unordered_map<SDL_Scancode, uint8_t> SDLKeyMap = {
		{ SDL_SCANCODE_X, 0x0 },
		{ SDL_SCANCODE_1, 0x1 },
		{ SDL_SCANCODE_2, 0x2 },
		{ SDL_SCANCODE_3, 0x3 },
		{ SDL_SCANCODE_Q, 0x4 },
		{ SDL_SCANCODE_W, 0x5 },
		{ SDL_SCANCODE_E, 0x6 },
		{ SDL_SCANCODE_A, 0x7 },
		{ SDL_SCANCODE_S, 0x8 },
		{ SDL_SCANCODE_D, 0x9 },
		{ SDL_SCANCODE_Z, 0xA },
		{ SDL_SCANCODE_C, 0xB },
		{ SDL_SCANCODE_4, 0xC },
		{ SDL_SCANCODE_R, 0xD },
		{ SDL_SCANCODE_F, 0xE },
		{ SDL_SCANCODE_V, 0xF }
	};
};