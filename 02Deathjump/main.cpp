#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION

#include <windows.h>	
#include <tchar.h>
#include "stb\stb_image.h"

#include "Constants.h"
#include "engine\Clock.h"
#include "Application.h"

extern float Globals::offset = 0.0f;
extern unsigned long Globals::CONTROLLS = 0;
extern unsigned long Globals::CONTROLLSHOLD = 0;
extern unsigned char Globals::pKeyBuffer[256] = {0};
extern Globals::POINT Globals::cursorPosScreen = {0, 0};
extern Globals::POINTF Globals::cursorPosNDC = {0.0f, 0.0f, 0.0f};
extern Globals::POINTF Globals::cursorPosEye = {0.0f, 0.0f, 0.0f };
extern bool Globals::lMouseButton = false;
extern Matrix4f Globals::projection = Matrix4f::IDENTITY;
extern Matrix4f Globals::invProjection = Matrix4f::IDENTITY;
extern AssetManager<Shader> Globals::shaderManager = AssetManager<Shader>();
extern AssetManager<CharacterSet> Globals::fontManager = AssetManager<CharacterSet>();
extern AssetManager<SoundBuffer> Globals::soundManager = AssetManager<SoundBuffer>();
extern AssetManager<MusicBuffer> Globals::musicManager = AssetManager<MusicBuffer>();
//becareful never combine extern and static it's just for playing around with the templates
extern AssetManagerStatic<Texture> Globals::textureManager = AssetManagerStatic<Texture>::get();
extern AssetManager<Spritesheet> Globals::spritesheetManager = AssetManager<Spritesheet>();

extern float Globals::bestTime = 0.0f;
extern float Globals::musicVolume = 0.1f;
extern float Globals::soundVolume = 0.3f;
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