#include "IOLayer.h"

extern bool halt;
extern int cycleDelay;
extern int videoScale;


IOLayer::IOLayer(char const* title, int width, int height, int textureWidth, int textureHeight) {
	windowWidth = width;
	windowHeight = height;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
	window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		throw;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	glContext = SDL_GL_CreateContext(window);


	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to load gl symbols using GLAD!" << std::endl;
		throw;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable multi-viewport

	ImGui::StyleColorsDark();
	
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");  // or whatever version you use

	glGenTextures(1, &framebufferTex);
	glBindTexture(GL_TEXTURE_2D, framebufferTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // pixel-perfect
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Allocate storage (e.g., 64x32 initially)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void IOLayer::Render(void* buffer, int pitch, bool* updateTexture, bool romLoaded) {
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Only update texture if draw flag is set
	if (*updateTexture) {
		*updateTexture = false;

		glBindTexture(GL_TEXTURE_2D, framebufferTex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 32, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	}

	// Start ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Optional: Set initial window size and position for each panel
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(640, 330)); // Display window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));     // Remove window padding
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));       // Remove spacing between items

	ImGui::Begin("Display", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar);

	ImGui::Image((ImTextureID)(intptr_t)framebufferTex, ImVec2(640, 320));
	ImGui::End();
	ImGui::PopStyleVar(2); // Restore styles

	// Settings Panel
	ImGui::SetNextWindowPos(ImVec2(640, 0));
	ImGui::SetNextWindowSize(ImVec2(360, 330)); // Adjust as needed

	ImGui::Begin("Settings", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);

	if (!romLoaded) {
		ImGui::Text("Please load a rom file!");
	}

	// Load ROM File
	if (ImGui::Button("Load ROM")) {
		const char* filters[] = { "*.ch8", "*.rom", "*.bin", "*" };
		const char* path = tinyfd_openFileDialog(
			"Select ROM File",
			"",
			4,
			filters,
			"CHIP-8 ROMs",
			0
		);

		if (path) {
			romPath = std::string(path);  // assuming you have romFile declared
			loadROM = true;
		}
	}
	// Reload ROM
	if (ImGui::Button("Reload ROM")) {
		loadROM = true;
	}

	// Pause Checkbox
	ImGui::Text("Pause:");
	ImGui::SameLine();
	ImGui::Checkbox("##Pause", &halt); // ## hides label from visible UI

	// Cycle Delay
	ImGui::Text("Cycle Time (ms):");
	ImGui::SameLine();
	ImGui::DragInt("##CycleDelay", &cycleDelay, 1, 0, 100);
	ImGui::End();

	// Debug Panel at bottom
	ImGui::SetNextWindowPos(ImVec2(0, 330));
	ImGui::SetNextWindowSize(ImVec2(1000, 370)); // Full width bottom panel

	ImGui::Begin("Debugger", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Window 3 - DBG info goes here...");
	ImGui::End();
	
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window);
	
	//ImGuiIO& io = ImGui::GetIO();
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//}
}

bool IOLayer::HandleEvent(uint8_t* pKeys) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL3_ProcessEvent(&event);  // Pass input to ImGui
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

IOLayer::~IOLayer() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	SDL_DestroyWindow(window);
	SDL_Quit();
}
