#include "LayerSDL.h"

LayerSDL::LayerSDL(char const* title, int width, int height, int textureWidth, int textureHeight) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow(title, width, height, 0);
	
	if (window == NULL) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return;
	}
	
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	renderer = SDL_CreateRenderer(window, NULL);

	mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
	SDL_SetTextureScaleMode(mainTexture, SDL_SCALEMODE_NEAREST);
}

void LayerSDL::Update(void* buffer, int pitch) {
	SDL_UpdateTexture(mainTexture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	
	// Set scale mode to nearest (pixel perfect)
	//SDL_SetTextureScaleMode(mainTexture, SDL_SCALEMODE_NEAREST);

	SDL_RenderTexture(renderer, mainTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool LayerSDL::HandleEvent(uint8_t* pKeys) {
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			return true;
		case SDL_EVENT_KEY_DOWN: 
		{
			if (event.key.scancode == SDL_SCANCODE_ESCAPE)
				return true;
			SDL_Scancode scanKey = event.key.scancode;
			if(SDLKeyMap.contains(scanKey)) {
				pKeys[SDLKeyMap.at(scanKey)] = 1; // Set key pressed
			}
		}
		break;
		case SDL_EVENT_KEY_UP:
		{
			SDL_Scancode scanKey = event.key.scancode;
			if (SDLKeyMap.contains(scanKey)) {
				pKeys[SDLKeyMap.at(scanKey)] = 0; // Set key up
			}
		}
			break;
		default:
			break;
		}
	}

	return false;
}

LayerSDL::~LayerSDL() {
	SDL_DestroyTexture(mainTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
