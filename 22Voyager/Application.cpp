#include <iostream>
#include <time.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#include "engine/Fontrenderer.h"
#include "Application.h"
#include "Constants.h"
#include "ShapeInterface.h"
#include "Game.h"
#include "MainMenu.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EventDispatcher& Application::EventDispatcher = EventDispatcher::Get();
StateMachine* Application::Machine = nullptr;
HWND Application::Window;
unsigned int Application::Width;
unsigned int Application::Height;
bool Application::InitWindow = false;
bool Application::Fullscreen = false;
bool Application::Init = false;
DWORD Application::SavedExStyle;
DWORD Application::SavedStyle;
RECT Application::Savedrc;
HCURSOR Application::Cursor = LoadCursor(nullptr, IDC_ARROW);
HANDLE Application::Icon = LoadImage(NULL, "res/icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
bool Application::VerticalSync = true;

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	Width = WIDTH;
	Height = HEIGHT;
	Framebuffer::SetDefaultSize(Width, Height);

	createWindow();
	initOpenGL();
	showWindow();
	initImGUI();
	initOpenAL();
	loadAssets();
	initStates();

	EventDispatcher.setProcessOSEvents([&]() {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return false;		
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	});

	SavedExStyle = GetWindowLong(Window, GWL_EXSTYLE);
	SavedStyle = GetWindowLong(Window, GWL_STYLE);

	Fontrenderer::Get().init();
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font"));

	auto shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_projection", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();
	Init = true;
}

Application::~Application() {
	Fontrenderer::Get().shutdown();

	delete Machine;
	Globals::shaderManager.clear();
	//release OpenGL context
	HDC hdc = GetDC(Window);
	wglMakeCurrent(GetDC(Window), 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(Window, hdc);
	//release OpenAL context
	SoundDevice::shutDown();

	UnregisterClass("WINDOWCLASS", (HINSTANCE)GetModuleHandle(NULL));
}

void Application::createWindow() {
	
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = StaticWndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	windowClass.hIcon = (HICON)Icon;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "WINDOWCLASS";
	windowClass.hIconSm = (HICON)Icon;

	if (!RegisterClassEx(&windowClass))
		return;

	Window = CreateWindowEx(
		NULL,
		"WINDOWCLASS",
		"Voyager",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		Width,
		Height,
		NULL,
		NULL,
		windowClass.hInstance,
		this);

	if (!Window)
		return;

}

void Application::showWindow() {
	RECT rect1;
	GetWindowRect(Window, &rect1);
	RECT rect2;
	GetClientRect(Window, &rect2);

	SetWindowPos(Window, NULL, rect1.left, rect1.top, Width + ((rect1.right - rect1.left) - (rect2.right - rect2.left)), Height + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)), NULL);

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	InitWindow = true;
}

LRESULT CALLBACK Application::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	Application* application = nullptr;

	switch (message) {
		case WM_CREATE: {
			application = static_cast<Application*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));
			break;
		}default: {
			application = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			break;
		}
	}

	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) return 0;

	if ((message == WM_KEYDOWN && wParam == VK_ESCAPE) || (message == WM_KEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN))) || (message == WM_SYSKEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN)))) {
		ImGui::GetIO().WantCaptureMouse = false;
	}

	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	if (InitWindow && ImGui::GetIO().WantCaptureMouse) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	if (application) {	
		application->processEvent(hWnd, message, wParam, lParam);
		return application->DisplayWndProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Application::DisplayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
		case WM_CREATE: {
			Keyboard::instance().enable();
			break;
		}case WM_DESTROY: {
			if (InitWindow) {
				PostQuitMessage(0);
			}
			break;
		}case WM_KEYDOWN: case WM_SYSKEYDOWN: {

			switch (wParam) {
				case VK_RETURN: {
					if ((HIWORD(lParam) & KF_ALTDOWN))
						ToggleFullScreen(!Fullscreen);
					break;
				}
			}
			break;
		}case WM_SIZE: {
			int deltaW = Width;
			int deltaH = Height;

			Height = HIWORD(lParam);		// retrieve width and height
			Width = LOWORD(lParam);

			deltaW = Width - deltaW;
			deltaH = Height - deltaH;

			if (Height == 0) {					// avoid divide by zero
				Height = 1;
			}
			Resize(deltaW, deltaH);
			if (Init) {
				Globals::musicManager.get("background").updateBufferStream();
			}
			break;
		}default: {
			//Mouse::instance().handleMsg(hWnd, message, wParam, lParam);
			break;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Application::initOpenGL(int msaaSamples) {

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context

	hDC = GetDC(Window);
	int nPixelFormat;					// our pixel format index

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// size of structure
		1,								// default version
		PFD_DRAW_TO_WINDOW |			// window drawing support
		PFD_SUPPORT_OPENGL |			// OpenGL support
		PFD_DOUBLEBUFFER,				// double buffering support
		PFD_TYPE_RGBA,					// RGBA color mode
		32,								// 32 bit color mode
		0, 0, 0, 0, 0, 0,				// ignore color bits, non-palettized mode
		0,								// no alpha buffer
		0,								// ignore shift bit
		0,								// no accumulation buffer
		0, 0, 0, 0,						// ignore accumulation bits
		24,								// 24 bit z-buffer size
		8,								// 8 bit stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main drawing plane
		0,								// reserved
		0, 0, 0 };						// layer masks ignored

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);	// choose best matching pixel format
	SetPixelFormat(hDC, nPixelFormat, &pfd);		// set pixel format to device context


	hRC = wglCreateContext(hDC);				// create rendering context and make it current
	wglMakeCurrent(hDC, hRC);
	glewInit();

	int MSAAPixelFormat = 0;

	if (msaaSamples > 0) {
		if (WGLEW_ARB_pixel_format && GLEW_ARB_multisample) {
			while (msaaSamples > 0) {
				UINT NumFormats = 0;

				int PFAttribs[] = {
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 32,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, msaaSamples,
					0
				};

				if (wglChoosePixelFormatARB(hDC, PFAttribs, NULL, 1, &MSAAPixelFormat, &NumFormats) == TRUE && NumFormats > 0) break;
				msaaSamples--;
			}
		}

		if (MSAAPixelFormat) {
			wglMakeCurrent(hDC, 0);
			wglDeleteContext(hRC);
			ReleaseDC(Window, hDC);
			DestroyWindow(Window);
			Window = NULL;
			UnregisterClass("WINDOWCLASS", (HINSTANCE)GetModuleHandle(NULL));


			createWindow();
			hDC = GetDC(Window);

			SetPixelFormat(hDC, MSAAPixelFormat == 0 ? nPixelFormat : MSAAPixelFormat, &pfd);
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);
		}

	}
	ToggleVerticalSync();

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Application::initImGUI() {
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window);
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	ImGui_ImplOpenGL3_Init("#version 410 core");
}

void Application::initOpenAL() {
	SoundDevice::init();
}

void Application::ToggleVerticalSync() {

	// WGL_EXT_swap_control.
	typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(GLint);
	static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(
			wglGetProcAddress("wglSwapIntervalEXT"));

	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(VerticalSync);
		VerticalSync = !VerticalSync;
	}
}

HWND Application::GetWindow() {
	return Window;
}

bool Application::isRunning() {
	return EventDispatcher.update();
}

void Application::render() {
	Machine->render();
}

void Application::update() {
	Mouse::instance().update();
	Keyboard::instance().update();

	Globals::musicManager.get("background").updateBufferStream();

	Machine->update();

	if (!Machine->isRunning()) {
		SendMessage(Window, WM_DESTROY, NULL, NULL);
	}
}

void Application::fixedUpdate() {
	Machine->fixedUpdate();
}

void Application::initStates() {
	
	Machine = new StateMachine(m_dt, m_fdt);
	//Machine->addStateAtTop(new Game(*Machine));
	//Mouse::instance().attach(Window);
	Machine->addStateAtTop(new MainMenu(*Machine));
	
	//Machine->addStateAtTop(new ShapeInterface(*Machine));
}

void Application::processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
		case WM_MOUSEMOVE: {
			Event event;
			event.type = Event::MOUSEMOTION;
			event.data.mouseMove.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseMove.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			EventDispatcher.pushEvent(event);
			break;
		}case WM_LBUTTONDOWN: {
			Event event;
			event.type = Event::MOUSEBUTTONDOWN;
			event.data.mouseButton.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseButton.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			event.data.mouseButton.button = Event::MouseButtonEvent::MouseButton::BUTTON_LEFT;
			EventDispatcher.pushEvent(event);
			break;
		}case WM_RBUTTONDOWN: {
			Event event;
			event.type = Event::MOUSEBUTTONDOWN;
			event.data.mouseButton.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseButton.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			event.data.mouseButton.button = Event::MouseButtonEvent::MouseButton::BUTTON_RIGHT;
			EventDispatcher.pushEvent(event);
			break;
		}case WM_LBUTTONUP: {
			Event event;
			event.type = Event::MOUSEBUTTONUP;
			event.data.mouseButton.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseButton.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			event.data.mouseButton.button = Event::MouseButtonEvent::MouseButton::BUTTON_LEFT;
			EventDispatcher.pushEvent(event);
			break;
		} case WM_RBUTTONUP: {
			Event event;
			event.type = Event::MOUSEBUTTONUP;
			event.data.mouseButton.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseButton.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			event.data.mouseButton.button = Event::MouseButtonEvent::MouseButton::BUTTON_RIGHT;
			EventDispatcher.pushEvent(event);
			break;
		} case WM_WINDOWPOSCHANGING: {
			if (Init) {
				Globals::musicManager.get("background").updateBufferStream();
			}
			break;
		}case WM_KEYDOWN: {

			switch (wParam) {

				case VK_ESCAPE: {
					Event event;
					event.type = Event::KEYDOWN;
					event.data.keyboard.keyCode = wParam;
					EventDispatcher.pushEvent(event);
					break;
				}case VK_SPACE: {
					Event event;
					event.type = Event::KEYDOWN;
					event.data.keyboard.keyCode = wParam;
					EventDispatcher.pushEvent(event);
					break;
				}case 'z': case 'Z': {
					Event event;
					event.type = Event::KEYDOWN;
					event.data.keyboard.keyCode = wParam;
					EventDispatcher.pushEvent(event);
					break;
				}case 'v': case 'V': {
					Event event;
					event.type = Event::KEYDOWN;
					event.data.keyboard.keyCode = wParam;
					EventDispatcher.pushEvent(event);
					break;
				}
			}
		}
	}
}

void Application::Resize(int deltaW, int deltaH) {
	glViewport(0, 0, Width, Height);
	
	if (Init) {
		Framebuffer::SetDefaultSize(Width, Height);

		Machine->resize(Width, Height);
	
		auto shader = Globals::shaderManager.getAssetPointer("font");
		shader->use();
		shader->loadMatrix("u_projection", Matrix4f::Orthographic(0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f));
		shader->unuse();

		Machine->m_states.top()->resize(deltaW, deltaH);
	}	
}

void Application::ToggleFullScreen(bool isFullScreen, unsigned int width, unsigned int height) {
	
	int deltaW = width == 0u ? Width : width;
	int deltaH = height == 0u ? Height : height;

	if (isFullScreen) {

		if (!Fullscreen) {
			GetWindowRect(Window, &Savedrc);
		}
		Fullscreen = true;
		SetWindowLong(Window, GWL_EXSTYLE, 0);
		SetWindowLong(Window, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		SetWindowPos(Window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		Width = width == 0u ?  GetSystemMetrics(SM_CXSCREEN) : width;
		Height = height == 0u ? GetSystemMetrics(SM_CYSCREEN): height;

		deltaW = Width - deltaW;
		deltaH = Height - deltaH;

		SetWindowPos(Window, HWND_TOPMOST, 0, 0, Width, Height, SWP_SHOWWINDOW);
		//Resize(deltaW, deltaH);
	}

	if (!isFullScreen) {
		
		Fullscreen = false;

		SetWindowLong(Window, GWL_EXSTYLE, SavedExStyle);
		SetWindowLong(Window, GWL_STYLE, SavedStyle);
		SetWindowPos(Window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		Width = (width == 0u || width == (Width + 16u)) ? Savedrc.right - Savedrc.left : width;
		Height = (height == 0u || height == (Height + 39u)) ? Savedrc.bottom - Savedrc.top : height;

		deltaW = Width - deltaW;
		deltaH = Height - deltaH;

		SetWindowPos(Window, HWND_NOTOPMOST, 0, 0, Width, Height, SWP_SHOWWINDOW);
		//Resize(deltaW, deltaH);
	}
}

void Application::SetCursorIconFromFile(std::string file) {
	Application::Cursor = LoadCursorFromFileA(file.c_str());
	SetCursor(Cursor);
	SetClassLongPtr(Window, GCLP_HCURSOR, (DWORD)Cursor);
}

void Application::SetCursorIcon(LPCSTR resource) {
	Application::Cursor = LoadCursor(nullptr, resource);
	SetCursor(Cursor);
	SetClassLongPtr(Window, GCLP_HCURSOR, (DWORD)Cursor);
}

void Application::loadAssets() {
	Globals::shaderManager.loadShader("terrain", "res/Shaders/TerrainVertexShader.vs", "res/Shaders/TerrainFragmentShader.fs");
	Globals::shaderManager.loadShader("muzzle", "res/Shaders/Muzzle Flash Shader/VertexShaderMuzzleFlash.vs", "res/Shaders/Muzzle Flash Shader/FragmentShaderMuzzleFlash.fs");
	Globals::shaderManager.loadShader("weapon", "res/Shaders/SingleModelLoader.vs", "res/Shaders/SingleModelLoader.fs");
	Globals::shaderManager.loadShader("instance", "res/Shaders/SingleModelLoader_ins.vs", "res/Shaders/SingleModelLoader.fs");
	Globals::shaderManager.loadShader("skybox", "res/Shaders/SkyboxVertexShader.vs", "res/Shaders/SkyboxFragmentShader.fs");
	Globals::shaderManager.loadShader("default", "res/Shaders/DefaultVertexShader.vs", "res/Shaders/DefaultFragmentShader.fs");
	Globals::shaderManager.loadShader("hud", "res/Shaders/HUD.vs", "res/Shaders/HUD.fs");

	Globals::shaderManager.loadShader("enemy", "res/Shaders/EnemyVertexShader.vs", "res/Shaders/EnemyFragmentShader.fs");
	Globals::shaderManager.loadShader("unlit", "res/Shaders/Unlit.vs", "res/Shaders/Unlit.fs");
	Globals::shaderManager.loadShader("font", "res/Shaders/batch.vs", "res/Shaders/font.fs");
	Globals::shaderManager.loadShader("particle", "res/Shaders/Particle System Shaders/VertexShader.vs", "res/Shaders/Particle System Shaders/FragmentShader.fs", "res/Shaders/Particle System Shaders/GeometryShader.geom");
	Globals::shaderManager.loadShader("post", "res/Shaders/PostProcessingVertexShader.vs", "res/Shaders/PostProcessingFragmentShader.fs");
	Globals::shaderManager.loadShader("debugger", "res/Shaders/Debugger.vs", "res/Shaders/Debugger.fs");

	Globals::shapeManager.buildQuadXY("quad", Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), 1, 1, true, false, false);
	Globals::shapeManager.buildCube("cube", Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), 1, 1, false, false, false);
	Globals::shapeManager.buildSphere("sphere", 3.0f, Vector3f(0.0f, 0.0f, 0.0f), 75, 75, true, true, false);

	std::string faces[] = { "res/Textures/Skyboxes/TitanMoon/right.png", "res/Textures/Skyboxes/TitanMoon/left.png", "res/Textures/Skyboxes/TitanMoon/top.png", "res/Textures/Skyboxes/TitanMoon/bottom.png", "res/Textures/Skyboxes/TitanMoon/back.png", "res/Textures/Skyboxes/TitanMoon/front.png", };
	Globals::textureManager.loadCubeMap("titan", faces, false);
	Globals::textureManager.loadTexture("muzzleFlash", "res/Textures/muzzleFlash.png", true);
	Globals::textureManager.loadTexture("saturn", "res/Textures/saturnTex.jpg", true);
	Globals::textureManager.loadTexture("saturnRings", "res/Textures/saturn_rings.png", true);
	Globals::textureManager.loadTexture("clothTex", "res/Textures/Camouflage.jpg", true);
	Globals::textureManager.loadTexture("grid", "res/grid512.png", true);
	Globals::textureManager.loadTexture("sniperScope", "res/Textures/SniperScope.png", true);
	Globals::textureManager.loadTexture("crossHair", "res/Textures/crossHair.png", true);
	Globals::textureManager.loadTexture("health", "res/Textures/HealthIcon.png", true);
	Globals::textureManager.loadTexture("ammo", "res/Textures/Ammo.png", true);
	Globals::textureManager.loadTexture("enemyTex", "res/Textures/enemy01.jpg", true);
	Globals::textureManager.loadTexture("drone", "res/Textures/drone.jpg", true);
	Globals::textureManager.loadTexture("shockwave", "res/Textures/shockwave.png", true);
	Globals::textureManager.loadTexture("redOrb", "res/Textures/RedOrb.png", true);

	Globals::textureManager.loadTexture("mainMenu", "res/Textures/menu.png", false);
	Globals::textureManager.loadTexture("indicator", "res/Textures/Indicator.png", false);
	Globals::textureManager.loadTexture("aboutMenu", "res/Textures/AboutBackground.png", false);
	Globals::textureManager.loadTexture("playerDead", "res/Textures/PlayerDeathScreen.png", false);
	Globals::textureManager.loadTexture("victorious", "res/Textures/VictoryScreen.png", false);
	 
	Globals::fontManager.loadCharacterSet("roboto_20", "res/Fonts/Roboto-BoldItalic.ttf", 20, 3, 20, 128, 2, true, 0u);
	Globals::fontManager.loadCharacterSet("roboto_28", "res/Fonts/Roboto-BoldItalic.ttf", 30, 1, 20, 128, 0, true, 1u);
	Globals::spritesheetManager.createSpritesheetFromTexture("font", Globals::fontManager.get("roboto_20").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("roboto_28").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->setLinear();

	MusicBuffer::Init();
	SoundBuffer::Init();

	Globals::musicManager.createMusicBuffer("background");
	Globals::musicManager.get("background").setVolume(Globals::musicVolume);
	Globals::musicManager.get("background").setLooping(true);

	Globals::soundManager.createSoundBuffer("player", 7u, 45u, Globals::soundVolume * 1.2f);
	Globals::soundManager.createSoundBuffer("enemy", 2u, 2u, Globals::soundVolume * 1.2f);
	Globals::soundManager.createSoundBuffer("effect", 1u, 1u, Globals::soundVolume * 1.2f);

	Globals::soundManager.createSoundBuffer("mainMenu", 0u, 2u, Globals::soundVolume * 1.2f);
	Globals::soundManager.get("mainMenu").loadChannel("res/Audio/ButtonHovered.wav", 0u);
	Globals::soundManager.get("mainMenu").loadChannel("res/Audio/ButtonClicked.wav", 1u);
}