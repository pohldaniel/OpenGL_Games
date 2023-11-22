#pragma once

#include <windows.h>
#include "Constants.h"
#include "Level.h"
#include "CharacterController.h"
#include "CharacterController_cs.h"

class Application {
public:
	Application(const float& dt, const float& fdt);
	~Application();

	void update();
	void fixedUpdate();
	void render();
	bool isRunning();
	HWND getWindow();
	
private:
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool initWindow();
	void initOpenGL();

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
	bool m_init = false;
	
	
	Level* m_level;
	CharacterController* m_characterController;
	CharacterControllerCS* m_characterControllerCS;
};
