#define NOMINMAX
#include <windows.h>	
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include "engine\Clock.h"
#include "Application.h"
#include "Constants.h"
#include "Random.h"

extern Matrix4f Globals::projection = Matrix4f();
extern Matrix4f Globals::invProjection = Matrix4f();
extern Matrix4f Globals::orthographic = Matrix4f();
extern Matrix4f Globals::invOrthographic = Matrix4f();
extern AssetManager<Shader> Globals::shaderManager = AssetManager<Shader>();
extern AssetManager<Texture> Globals::textureManager = AssetManager<Texture>();
extern AssetManager<Spritesheet> Globals::spritesheetManager = AssetManager<Spritesheet>();
extern AssetManager<CharacterSet> Globals::fontManager = AssetManager<CharacterSet>();
extern AssetManager<SoundBuffer> Globals::soundManager = AssetManager<SoundBuffer>();
extern AssetManager<MusicBuffer> Globals::musicManager = AssetManager<MusicBuffer>();
extern AssetManager<SoundEffectsPlayer> Globals::soundEffectsPlayer = AssetManager<SoundEffectsPlayer>();

extern bool Globals::enableWireframe = false;
extern bool Globals::savingAllowed = true;
extern bool Globals::isPaused = false;
extern Clock Globals::clock = Clock();

extern const unsigned int Globals::viewBinding = 4;
extern unsigned int Globals::viewUbo = 0;

extern float Globals::soundVolume = 0.2f;
extern float Globals::musicVolume = 0.2f;
extern bool Globals::applyDisplaymode = false;
extern bool Globals::fullscreen = false;
extern unsigned int Globals::width = 1024;
extern unsigned int Globals::height = 768;

extern void Globals::useDisplaymode(bool flag) {
	Globals::applyDisplaymode = flag;
}

extern void Globals::setSoundVolume(float soundVolume) {
	Globals::soundVolume = soundVolume;
}

extern void Globals::setMusicVolume(float musicVolume) {
	Globals::musicVolume = musicVolume;
}

extern void Globals::setResolution(unsigned int width, unsigned int height) {
	Globals::width = width;
	Globals::height = height;
}

extern void  Globals::isFullscreen(bool flag) {
	Globals::fullscreen = flag;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");
	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d  : move character" << std::endl;
	std::cout << "c           : open character info" << std::endl;
	std::cout << "b           : open spellbook" << std::endl;
	std::cout << "i           : open inventory" << std::endl;
	std::cout << "u           : open shop" << std::endl;
	std::cout << "q           : open questlog" << std::endl;
	std::cout << "f           : test message" << std::endl;
	std::cout << "Esc         : in game option" << std::endl;
	std::cout << "alt + enter : fullscreen" << std::endl;
	#endif
	
	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;

	RNG::initRNG(time(0));

	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = application.getWindow();
	HDC hdc = GetDC(hwnd);

	Clock deltaClock;
	Clock fixedDeltaClock;

	int frames = 0;
	float framesTime = 0;

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

		SwapBuffers(hdc);
	}
	
}