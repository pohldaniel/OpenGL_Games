#pragma once
#include <windows.h>
#include <memory>
#include <engine/input/Event.h>
#include <engine/input/EventDispatcher.h>
#include <engine/input/Keyboard.h>
#include <engine/input/Mouse.h>
#include <States/StateMachine.h>

#include <NsGui/IRenderer.h>
#include <Event/EventEmitter.h>

#include <System/AttackSystem.h>
#include <System/render-system.hpp>
#include <System/movement-system.hpp>
#include <System/animation-system.hpp>
#include <System/wave-system.hpp>
#include <System/life-and-death-system.hpp>

#include <glm/gtx/transform.hpp>

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
	static void SetCursorIcon(HCURSOR cursor);
	static const HWND& GetWindow();

	static int Width;
	static int Height;

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void createWindow();
	void initOpenGL(int msaaSamples = 0);
	void showWindow();
	void initImGUI();
	static void initNoesisGUI();
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
	static HICON Icon;
	static bool VerticalSync;
	static bool Fullscreen;

	static Noesis::Ptr<Noesis::RenderDevice> NoesisDevice;
	static EventEmitter Emitter;
	static entt::DefaultRegistry Registry;
	static RenderSystem* s_RenderSystem;
	static AnimationSystem* s_AnimationSystem;
	static MovementSystem* s_MovementSystem;
	static WaveSystem* s_WaveSystem;
	static AttackSystem* s_AttackSystem;
	static LifeAndDeathSystem* s_LifeAndDeathSystem;
	static Progression s_Progression;
	static Level* s_Level;

	glm::mat4 projection;
	glm::mat4 view;
	glm::vec2 viewTranslation;
	float viewScale;
};