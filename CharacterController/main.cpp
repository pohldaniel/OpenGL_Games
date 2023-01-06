#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION
#include <windows.h>	
#include <tchar.h>

#include "Constants.h"
#include "engine\Clock.h"
#include "Application.h"

extern float Globals::offset = 0.0f;
extern unsigned long Globals::CONTROLLS = 0;
extern unsigned long Globals::CONTROLLSHOLD = 0;
extern unsigned char Globals::pKeyBuffer[256] = { 0 };
extern Matrix4f Globals::projection = Matrix4f::IDENTITY;

extern b2World* Globals::world = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d    : move character" << std::endl;
	std::cout << "q             : change state" << std::endl;
	std::cout << "v             : toggle vsync" << std::endl;
	std::cout << "z             : toggle wireframe" << std::endl;
#endif

	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	double physicsElapsedTime = 0.0;

	float gravityVal = -1 * ((2 * MAX_JUMP_HEIGHT) / powf(JUMP_APEX_TIME, 2.f));
	b2Vec2 gravity(0.0f, gravityVal);
	Globals::world = new b2World(gravity);

	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = application.getWindow();
	HDC hdc;

	Clock deltaClock;
	Clock fixedDeltaClock;

	int frames = 0;
	double framesTime = 0;

	while (application.isRunning()) {

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
		}
#endif
		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);
	}
	return 0;
}