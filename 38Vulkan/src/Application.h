#pragma once
#include <windows.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_vulkan.h>

#include <engine/input/Event.h>
#include <engine/input/EventDispatcher.h>
#include <engine/input/Keyboard.h>
#include <engine/input/Mouse.h>
#include <States/StateMachine.h>
#include "Vulkan/VlkContext.h"

class Application {

public:
	Application(const float& dt, const float& fdt);
	~Application();

	void update();
	void fixedUpdate();
	void render();
	bool isRunning();

	static void ToggleFullScreen(bool isFullScreen, unsigned int width = 0, unsigned int height = 0);
	static void SetCursorIconFromFile(std::string file);
	static void SetCursorIcon(LPCSTR resource);
	static void SetCursorIcon(HCURSOR cursor);
	static const HWND& GetWindow();
	static StateMachine* GetMachine();

	static int Width;
	static int Height;
	//static VlkContext VlkContext;

	VkDescriptorPool imguiPool;

private:

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT ApplicationWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void createWindow();
	void showWindow();
	void initVulkan();
	void initImGUI();
	void loadAssets();
	void initStates();
	
	MSG msg;

	const float& m_fdt;
	const float& m_dt;

	void processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
	static HICON Icon;
	static bool Fullscreen;
};