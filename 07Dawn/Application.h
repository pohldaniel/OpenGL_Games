#pragma once
#include <windows.h>

#include "engine/Extension.h"
#include "engine/input/Event.h"
#include "engine/input/EventDispatcher.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Batchrenderer.h"
#include "engine/Fontrenderer.h"

#include "Constants.h"
#include "StateMachine.h"
#include "ViewPort.h"

class Application {
public:
	Application(const float& dt, const float& fdt);
	~Application();

	void update();
	void fixedUpdate();
	void render();
	bool isRunning();
	HWND getWindow();
	
	static void AddMouseListener(MouseEventListener * el);

private:
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool initWindow();
	void initOpenGL();
	void loadAssets();
	void initStates();

	void enableVerticalSync(bool enableVerticalSync);

	bool m_enableVerticalSync;

	MSG msg;
	HWND m_window;

	const float& m_fdt;
	const float& m_dt;
	bool m_init = false;

	StateMachine* m_machine;

	static EventDispatcher& s_eventDispatcher;
	void processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool m_mouseTracking = false;
};