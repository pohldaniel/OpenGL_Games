#pragma once
#include <windows.h>

#include "engine/input/Event.h"
#include "engine/input/EventDispatcher.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"

#include "StateMachine.h"


class Application {
public:
	Application(const float& dt, const float& fdt);
	~Application();

	void update();
	void fixedUpdate();
	void render();
	bool isRunning();
	HWND getWindow();

	static void ToggleFullScreen(bool isFullScreen, unsigned int width = 0, unsigned int height = 0);
	static unsigned int Width;
	static unsigned int Height;
	static bool Fullscreen;
	static void SetCursorIcon(std::string file);
private:

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void createWindow();
	void initOpenGL(int msaaSamples = 0);
	void showWindow();
	void initImGUI();
	void loadAssets();
	void initStates();

	void enableVerticalSync(bool enableVerticalSync);

	bool m_enableVerticalSync;

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
	static DWORD SavedExStyle;
	static DWORD SavedStyle;
	static RECT Savedrc;
	static DEVMODE DefaultScreen;
};