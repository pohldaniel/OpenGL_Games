#pragma once

#include <windows.h>
#include "Extension.h"
#include "Constants.h"
#include "StateMachine.h"

#include "Transition.h"

#include "Menu.h"
#include "Pause.h"
#include "Settings.h"



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

private:
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool initWindow();
	void initOpenGL();
	void loadTextures();

	void enableWireframe(bool enableWireframe);
	void enableVerticalSync(bool enableVerticalSync);
	void setCursortoMiddle(HWND hWnd);
	void processInput();

	POINT m_cursorPosScreenSpace;
	bool m_enableVerticalSync;
	bool m_enableWireframe;
	HWND m_window;
	MSG msg;


	const float& m_fdt;
	const float& m_dt;
	bool m_windowGrabbed = false;

	StateMachine* m_machine;

	
	Vector4f m_cursorPosEye;

	Menu* menu;
};
