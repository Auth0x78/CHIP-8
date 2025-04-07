#pragma once
#include <iostream>
#include <unordered_map>
#include <CHIP-8.h>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <glad/glad.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "tinyfiledialogs.h" // include after adding it to your project


class IOLayer {
public:
	IOLayer(char const* title, int width, int height, int textureWidth, int textureHeight);
	void Render(void* buffer, int pitch, bool* updateTexture, bool romLoaded);
	bool HandleEvent(uint8_t* pKeys);

	~IOLayer();
public:
	int windowWidth, windowHeight;
	bool loadROM = false;
	std::string romPath;

private:
	SDL_Window* window;
	SDL_GLContext glContext;
	GLuint framebufferTex;

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