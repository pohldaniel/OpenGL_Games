#pragma once

#include <windows.h>
#include "Extension.h"
#include "Constants.h"
#include "StateMachine.h"
#include "Game.h"

class Application {
public:
	Application(const float& dt, const float& fdt);
	~Application();

	void Update();
	/*void FixedUpdate();*/
	void render();
	bool isRunning();
	HWND getWindow();
	void initStates();

private:
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool initWindow();
	void initOpenGL();
	

	void enableWireframe(bool enableWireframe);
	void enableVerticalSync(bool enableVerticalSync);
	void setCursortoMiddle(HWND hWnd);
	void processInput();

	POINT m_oldCursorPos;
	bool m_enableVerticalSync;
	bool m_enableWireframe;
	HWND m_window;
	MSG msg;


	const float& m_fdt;
	const float& m_dt;
	bool m_windowGrabbed = false;

	StateMachine* m_machine;
	
};
