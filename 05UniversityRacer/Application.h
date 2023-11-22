#pragma once
#include <windows.h>

#include "engine/input/KeyBorad.h"
#include "engine/Extension.h"

#include "Constants.h"
#include "StateMachine.h"
#include "Game.h"

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
	void loadAssets();
	void initStates();

	void enableWireframe(bool enableWireframe);
	void enableVerticalSync(bool enableVerticalSync);

	bool m_enableVerticalSync;
	bool m_enableWireframe;

	MSG msg;
	HWND m_window;

	const float& m_fdt;
	const float& m_dt;
	bool m_init = false;

	StateMachine* m_machine;
};