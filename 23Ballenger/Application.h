#pragma once
#include <windows.h>

#include "engine/input/Event.h"
#include "engine/input/EventDispatcher.h"
#include "engine/input/Keyboard.h"
#include "engine/input/Mouse.h"
#include "engine/sound/SoundDevice.h"

#include "StateMachine.h"

#define SCALE	1.0f

class Application {

public:
	Application(const float& dt, const float& fdt);
	~Application();

	void update();
	void fixedUpdate();
	void render();
	bool isRunning();

	static void ToggleFullScreen(bool isFullScreen, unsigned int width = 0, unsigned int height = 0);
	static void ToggleVerticalSync();
	static void SetCursorIconFromFile(std::string file);
	static void SetCursorIcon(LPCSTR resource);
	static HWND GetWindow();

	static unsigned int Width;
	static unsigned int Height;

private:

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void createWindow();
	void initOpenGL(int msaaSamples = 0);
	void showWindow();
	void initImGUI();
	void initOpenAL();
	void loadAssets();
	void initStates();

	MSG msg;

	const float& m_fdt;
	const float& m_dt;

	void processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool m_mouseTracking = false;

	static void Resize(int deltaW, int deltaH);
	static StateMachine* Machine;
	static EventDispatcher& EventDispatcher;

	static HWND Window;
	static bool InitWindow;	
	static bool Init;
	static DWORD SavedExStyle;
	static DWORD SavedStyle;
	static RECT Savedrc;
	static DEVMODE DefaultScreen;
	static HCURSOR Cursor;
	static HANDLE Icon;
	static bool VerticalSync;
	static bool Fullscreen;
};