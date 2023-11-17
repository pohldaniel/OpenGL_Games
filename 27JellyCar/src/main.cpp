#include <windows.h>	
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <engine/Clock.h>
#include <Physics/DebugDrawer.h>
#include "ThreadCpp.h"

#include "Application.h"
#include "Globals.h"

extern AssetManager<Shader> Globals::shaderManager = AssetManager<Shader>();
extern AssetManager<Texture> Globals::textureManager = AssetManager<Texture>();
extern AssetManager<Shape> Globals::shapeManager = AssetManager<Shape>();
extern AssetManager<CharacterSet> Globals::fontManager = AssetManager<CharacterSet>();
extern AssetManager<Spritesheet> Globals::spritesheetManager = AssetManager<Spritesheet>();
extern AssetManager<SoundBuffer> Globals::soundManager = AssetManager<SoundBuffer>();
extern AssetManager<MusicBuffer> Globals::musicManager = AssetManager<MusicBuffer>();

extern float Globals::soundVolume = 0.2f;
extern float Globals::musicVolume = 0.1f;
extern Physics* Globals::physics = NULL;

DebugDrawer debugDrawer;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "right mouse      : capture game" << std::endl;
	std::cout << "alt              : toggle ui" << std::endl;
	std::cout << "ESC              : quit" << std::endl;
	std::cout << "v                : toggle vsync" << std::endl;
	std::cout << "alt + enter      : fullscreen" << std::endl;

	int frames = 0;
	float frameTime = 0;
#endif
	auto looper = std::make_unique<CLooper>();

	std::cout << "Starting looper" << std::endl;
	// To start and run
	looper->run();

	std::this_thread::sleep_for(std::chrono::seconds(5));

	std::cout << "Stopping looper" << std::endl;
	// To stop it and clean it up
	looper->stop();
	looper = nullptr;
	/*auto looper = std::make_unique<CLooper>();

	std::cout << "Starting looper" << std::endl;
	// To start and run
	looper->run();

	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	auto dispatcher = looper->getDispatcher();

	std::cout << "Adding tasks" << std::endl;

	for (uint32_t k = 0; k<500; ++k)
	{
		auto const task = [k]()
		{
			std::cout << "Invocation " << k
				<< ": Hello, I have been executed asynchronously on the looper for " << (k + 1)
				<< " times." << std::endl;
		};

		dispatcher->post(std::move(task));
	}

	std::cout << "Waiting 5 seconds for completion" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));

	std::cout << "Stopping looper" << std::endl;
	// To stop it and clean it up
	dispatcher = nullptr;*/
	
	


	Globals::physics = new Physics(PHYSICS_STEP);
	Physics::GetDynamicsWorld()->setDebugDrawer(&debugDrawer);

	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;
	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = Application::GetWindow();
	HDC hdc;

	Clock deltaClock;
	Clock fixedDeltaClock;

	while (application.isRunning()) {

		physicsElapsedTime += deltaTime;
		while (physicsElapsedTime > PHYSICS_STEP) {
			fixedDeltaTime = fixedDeltaClock.resetSec();
			if (fixedDeltaTime > PHYSICS_STEP * 5.0f)
				fixedDeltaTime = PHYSICS_STEP;

			application.fixedUpdate();
			physicsElapsedTime -= PHYSICS_STEP;
		}


		application.update();
		application.render();

		deltaTime = deltaClock.resetSec();

#if DEBUG
		frameTime += deltaTime;
		frames++;
		if (frameTime > 1) {
			_TCHAR fpsText[32];
			_sntprintf(fpsText, 32, "FPS: %d FPS", frames);
			SetWindowText(hwnd, fpsText);
			frames = 0;
			frameTime = 0;
			deltaClock.restart();
		}
#endif

		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);
	}

	delete Globals::physics;

	//looper->stop();
	//looper = nullptr;
	return 0;
}