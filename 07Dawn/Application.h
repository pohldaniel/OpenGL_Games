#pragma once
#include <windows.h>

#include "engine/Extension.h"
#include "engine/input/Event.h"
#include "engine/input/EventDispatcher.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/sound/SoundDevice.h"
#include "engine/Batchrenderer.h"
#include "engine/Fontrenderer.h"

#include "Constants.h"
#include "StateMachine.h"
#include "ViewPort.h"


class Application {
	friend class LoadingManager;

public:

	Application(const float& dt, const float& fdt);
	~Application();

	void update();
	void fixedUpdate();
	void render();
	bool isRunning();
	HWND getWindow();
	
	static void AddStateAtTop(State* state);
	static StateMachine& GetStateMachine();
	static void GetScreenMode(std::vector<DEVMODE>& list);
	static void ResetDisplayMode();
	static void SetDisplayMode(DEVMODE& settings);
	static void SetDisplayMode(unsigned int width, unsigned int height);
	static void ToggleFullScreen(bool isFullScreen, unsigned int width = 0, unsigned int height = 0);
	static void WriteConfigurationToFile();

private:
	
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool initWindow();
	void initOpenGL();
	void initOpenAL();
	void loadAssets();
	void initStates();
	static void resize(int deltaW, int deltaH);
	void processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void enableVerticalSync(bool enableVerticalSync);


	bool m_enableVerticalSync;
	MSG msg;
	

	const float& m_fdt;
	const float& m_dt;
	
	bool m_mouseTracking = false;

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	static StateMachine* s_machine;
	static EventDispatcher& s_eventDispatcher;
	static HGLRC MainContext;
	static HGLRC LoaderContext;
	static HWND Window;
	
	static bool Init;
	static unsigned int Width;
	static unsigned int Height;
	static bool Fullscreen;
	static DWORD SavedExStyle;
	static DWORD SavedStyle;
};