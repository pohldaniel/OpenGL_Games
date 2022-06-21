#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <windows.h>	
#include <tchar.h>
#include "stb\stb_image.h"
#include "stb\stb_image_write.h"
#include "Constants.h"
#include "engine\Clock.h"
#include "Application.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;


extern Matrix4f Globals::projection = Matrix4f::IDENTITY;
extern Matrix4f Globals::invProjection = Matrix4f::IDENTITY;
extern AssetManager<Shader> Globals::shaderManager = AssetManager<Shader>();
extern AssetManager<Texture> Globals::textureManager = AssetManager<Texture>();
extern AssetManager<Spritesheet> Globals::spritesheetManager = AssetManager<Spritesheet>();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d    : scroll map" << std::endl;
	std::cout << "v             : toggle vsync" << std::endl;
	std::cout << "z             : toggle wireframe" << std::endl;
	#endif

	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;

	Application application(deltaTime, fixedDeltaTime);
	
	HWND hwnd = application.getWindow();
	HDC hdc;

	Clock deltaClock;
	Clock fixedDeltaClock;
	
	int frames = 0;
	float framesTime = 0;
	
	while (application.isRunning()) {
		#if FIXEDUPDATE
		float timeToSleep = UPDATE_STEP - deltaTime;	
		if (timeToSleep > 0){	
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32_t>(timeToSleep * 1000.0f)));					
		}
		fixedDeltaTime = deltaTime * 0.8f;
		application.fixedUpdate();
		application.update();
		application.render();
		#else 
		physicsElapsedTime += deltaTime;
		while (physicsElapsedTime > PHYSICS_STEP) {
			fixedDeltaTime = fixedDeltaClock.restartSec();
			if (fixedDeltaTime > PHYSICS_STEP * 5.0f)
				fixedDeltaTime = PHYSICS_STEP;

			application.fixedUpdate();
			physicsElapsedTime -= PHYSICS_STEP;
		}

		application.update();
		application.render();
		#endif

		deltaTime = deltaClock.restartSec();

		#if DEBUG
		framesTime += deltaTime;
		frames++;
		if (framesTime > 1) {
			_TCHAR fpsText[32];
			_sntprintf(fpsText, 32, "FPS: %d FPS", frames);
			SetWindowText(hwnd, fpsText);
			frames = 0;
			framesTime = 0;
			deltaClock.restart();
		}
		
	
		#endif
		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);		
	}
	return 0;
}