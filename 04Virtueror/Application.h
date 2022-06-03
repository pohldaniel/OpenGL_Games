#pragma once

#include <windows.h>
#include <queue>

#include "engine/Extension.h"
#include "engine/input/Event.h"
#include "engine/input/EventDispatcher.h"
#include "engine/input/MouseEventListener.h"
#include "engine/input/Mouse.h"
#include "engine/input/KeyBorad.h"

#include "Constants.h"
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
	void initStates();

	void AddMouseListener(MouseEventListener * el);

private:
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool initWindow();
	void initOpenGL();
	void loadAssets();

	void enableWireframe(bool enableWireframe);
	void enableVerticalSync(bool enableVerticalSync);

	POINT m_cursorPosScreenSpace;
	bool m_enableVerticalSync;
	bool m_enableWireframe;
	
	MSG msg;
	HWND m_window;

	const float& m_fdt;
	const float& m_dt;
	bool m_windowGrabbed = false;
	bool m_init = false;
	StateMachine* m_machine;

	EventDispatcher * m_eventDispatcher = nullptr;
	void processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
