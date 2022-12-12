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

private:
	
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool initWindow();
	void initOpenGL();
	void loadAssets();
	void initStates();
	void resize(int deltaW, int deltaH);
	void processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void enableVerticalSync(bool enableVerticalSync);
	void toggleFullScreen();

	bool m_enableVerticalSync;
	bool m_isFullScreen;
	MSG msg;

	const float& m_fdt;
	const float& m_dt;
	bool m_init = false;
	unsigned int m_width;
	unsigned int m_height;

	
	bool m_mouseTracking = false;

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	static StateMachine* s_machine;
	static EventDispatcher& s_eventDispatcher;
	static HGLRC MainContext;
	static HGLRC LoaderContext;
	static HWND Window;
};