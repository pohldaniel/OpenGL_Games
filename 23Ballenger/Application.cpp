#include <iostream>
#include <time.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#include "engine/Framebuffer.h"
#include "engine/Fontrenderer.h"
#include "Application.h"
#include "Globals.h"
#include "ShapeInterface.h"
#include "Game.h"
#include "TerrainCulling.h"
#include "CloudInterface.h"
#include "Environment.h"
#include "Menu.h"
#include "Settings.h"
#include "Controls.h"
#include "Widget.h"

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
	Widget::Resize(Width, Height);

	createWindow();
	initOpenGL();
	showWindow();
	initImGUI();
	initOpenAL();
	loadAssets();
	

	EventDispatcher.setProcessOSEvents([&]() {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return false;		
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	});
	Init = true;
	SavedExStyle = GetWindowLong(Window, GWL_EXSTYLE);
	SavedStyle = GetWindowLong(Window, GWL_STYLE);

	Fontrenderer::Get().init();
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font"));

	auto shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();

	initStates();
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
	
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = StaticWndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "WINDOWCLASS";
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&windowClass))
		return;

	Window = CreateWindowEx(
		NULL,
		"WINDOWCLASS",
		"Ballenger",
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

	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) {
		
		return 0;
	}

	if ((message == WM_KEYDOWN && (wParam == 'v' || wParam == 'V')) || (message == WM_KEYDOWN && wParam == VK_ESCAPE) || (message == WM_KEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN))) || (message == WM_SYSKEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN)))) {
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
		}case WM_KEYDOWN: {
			switch (wParam) {
				case VK_RETURN: {
					if ((HIWORD(lParam) & KF_ALTDOWN))
						ToggleFullScreen(!Fullscreen);
					break;
				}
			}
			break;
		//ignore left alt
		}/*case WM_SYSKEYDOWN: {
			switch (wParam) {
				case VK_RETURN: {
					if ((HIWORD(lParam) & KF_ALTDOWN))
						ToggleFullScreen(!Fullscreen);
					break;
				}
			}
			break;
		}*/case WM_SIZE: {
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
	//glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	//Machine->addStateAtTop(new ShapeInterface(*Machine));
	//Machine->addStateAtTop(new TerrainCulling(*Machine));
	//Machine->addStateAtTop(new CloudInterface(*Machine));
	//Machine->addStateAtTop(new EnvironmentInterface(*Machine));
	//Machine->addStateAtTop(new Controls(*Machine));

	Machine->addStateAtTop(new Menu(*Machine));
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
				}
				/*case VK_MENU: {
					Event event;
					event.type = Event::KEYDOWN;
					event.data.keyboard.keyCode = VK_RMENU;
					EventDispatcher.pushEvent(event);
					break;
				}*/
#if DEBUGCOLLISION
				case 'z': case 'Z': {
					StateMachine::ToggleWireframe();
					break;
				}case 'v': case 'V': {
					ToggleVerticalSync();
					break;
				}
#endif
			}
			break;
		}case WM_SYSKEYDOWN: {
			switch (wParam) {
				case VK_MENU: {
					Event event;
					event.type = Event::KEYDOWN;
					event.data.keyboard.keyCode = VK_LMENU;
					EventDispatcher.pushEvent(event);
					break;
				}		
			}
			break;
		}case WM_MOUSEWHEEL: {
			Event event;
			event.type = Event::MOUSEWHEEL;
			event.data.mouseWheel.direction = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? Event::MouseWheelEvent::WheelDirection::UP : Event::MouseWheelEvent::WheelDirection::DOWN;
			
			EventDispatcher.pushEvent(event);

			//Mouse::instance().updateWheelDelta(static_cast<int>(static_cast<int>(wParam) >> 16));
			break;
		}
	}
}

void Application::Resize(int deltaW, int deltaH) {
	glViewport(0, 0, Width, Height);
	
	if (Init) {
		Framebuffer::SetDefaultSize(Width, Height);
		Widget::Resize(Width, Height);

		Machine->resize(Width, Height);
		Machine->m_states.top()->resize(deltaW, deltaH);

		auto shader = Globals::shaderManager.getAssetPointer("font");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f));
		shader->unuse();
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

	Globals::shaderManager.loadShader("terrain", "res/shader/terrain.vert", "res/shader/terrain.frag");
	Globals::shaderManager.loadShader("texture", "res/shader/texture.vert", "res/shader/texture.frag");
	Globals::shaderManager.loadShader("instance", "res/shader/instance.vert", "res/shader/instance.frag");
	Globals::shaderManager.loadShader("respawn", "res/shader/respawn.vert", "res/shader/respawn.frag");
	Globals::shaderManager.loadShader("cylinder", "res/shader/cylinder.vert", "res/shader/cylinder.frag");
	Globals::shaderManager.loadShader("disk", "res/shader/disk.vert", "res/shader/disk.frag");
	Globals::shaderManager.loadShader("column", "res/shader/column.vert", "res/shader/column.frag");
	Globals::shaderManager.loadShader("energy", "res/shader/energy.vert", "res/shader/energy.frag");
	Globals::shaderManager.loadShader("portal", "res/shader/portal.vert", "res/shader/portal.frag");
	Globals::shaderManager.loadShader("key", "res/shader/key.vert", "res/shader/key.frag");
	Globals::shaderManager.loadShader("beam", "res/shader/beam.vert", "res/shader/beam.frag");
	Globals::shaderManager.loadShader("skybox", "res/shader/skybox.vert", "res/shader/skybox.frag");
	Globals::shaderManager.loadShader("line", "res/shader/line.vert", "res/shader/line.frag");
	Globals::shaderManager.loadShader("player", "res/shader/player.vert", "res/shader/player.frag");

	//cloud post
	Globals::shaderManager.loadShader("quad_back", "res/shader/quad_back.vert", "res/shader/quad.frag");
	Globals::shaderManager.loadShader("quad", "res/shader/quad.vert", "res/shader/quad.frag");

	//UI
	Globals::shaderManager.loadShader("quad_color_uniform", "res/shader/quad_color_uniform.vert", "res/shader/quad_color_uniform.frag");
	Globals::shaderManager.loadShader("font", "res/shader/batch.vert", "res/shader/font.frag");
	Globals::shaderManager.loadShader("seeker", "res/shader/batch.vert", "res/shader/seeker.frag");


	Globals::textureManager.createNullTexture("null");

	Globals::textureManager.loadTexture("grass", "res/textures/grass.png", true);
	Globals::textureManager.loadTexture("rock", "res/textures/rock.png", true);

	Globals::textureManager.get("grass").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("grass").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.get("rock").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("rock").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.loadTexture("player", "res/textures/player.png", true);
	Globals::textureManager.loadTexture("player_nmp", "res/textures/playerNmap.png", true);

	Globals::textureManager.get("player").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("player").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.get("player_nmp").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("player_nmp").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.loadTexture("lava", "res/textures/lava.png", true);
	Globals::textureManager.get("lava").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("lava").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	
	Globals::textureManager.loadTexture("circle_off", "res/textures/circle_off.png", true);
	Globals::spritesheetManager.createSpritesheetFromTexture("circle", Globals::textureManager.get("circle_off").getTexture());

	Globals::textureManager.get("circle_off").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("circle_off").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.loadTexture("circle_on", "res/textures/circle_on.png", true);
	Globals::spritesheetManager.getAssetPointer("circle")->addToSpritesheet(Globals::textureManager.get("circle_on").getTexture());
	

	Globals::textureManager.get("circle_on").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("circle_on").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::spritesheetManager.getAssetPointer("circle")->setLinearMipMap();
	Globals::spritesheetManager.getAssetPointer("circle")->setRepeat();


	Globals::textureManager.loadTexture("column", "res/textures/column.png", false);
	Globals::textureManager.loadTexture("column_nmp", "res/textures/columnNmap.png", false);
	Globals::textureManager.get("column").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("column_nmp").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.loadTexture("portal", "res/textures/portal.png", false);
	Globals::textureManager.loadTexture("portal_nmp", "res/textures/portalNmap.png", false);
	Globals::textureManager.get("portal").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("portal_nmp").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.loadTexture("vortex", "res/textures/vortex.png", false);
	Globals::textureManager.get("vortex").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("vortex").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.loadTexture("key", "res/textures/key.png", false);
	Globals::textureManager.loadTexture("key_nmp", "res/textures/keyNmap.png", false);
	Globals::textureManager.get("key").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("key_nmp").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.loadCubeMapFromCross("skybox", "res/textures/skybox.png", true);

	Globals::shapeManager.buildCube("cube", Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), 1, 1, true, true, false);
	Globals::shapeManager.buildSphere("sphere", 0.5f, Vector3f(0.0f, 0.0f, 0.0f), 16, 16, true, true, false);
	Globals::shapeManager.buildQuadXY("quad", Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), 1, 1, true, true, true);
	Globals::shapeManager.buildQuadXZ("quad_lava", Vector3f(0.0f, 0.0f, 0.0f), Vector2f(1024.0f, 1024.0f), 1, 1, true, true, false);
	Globals::shapeManager.buildQuadXZ("quad_rp", Vector3f(-2.0f, 0.05f, -2.0f), Vector2f(4.0f, 4.0f), 1, 1, true, true, false);

	Globals::shapeManager.buildQuadXY("vortex", Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), 1, 1, true, true, false);
	Globals::shapeManager.buildCylinder("cylinder", 2.0f, 2.0f, 3.0f, Vector3f(0.0f, 1.6f, 0.0f), false, false, 16, 16, false, true, false);
	Globals::shapeManager.buildDiskXZ("disk", 2.0f, Vector3f(0.0f, 0.11f, 0.0f), 16, 16, false, false, false);
	Globals::shapeManager.fromObj("key", "res/models/key.obj");
	Globals::shapeManager.fromObj("column", "res/models/column.obj");
	Globals::shapeManager.fromObj("portal", "res/models/portal.obj");
	Globals::shapeManager.buildCylinder("cylinder_key", 2.0f, 2.0f, 1.0f, Vector3f(0.0f, 0.5f, 0.0f), false, false , 16, 16, false, true, false);
	Globals::shapeManager.buildQuadXZ("platform", Vector3f(-512.0f, 0.0f, -512.0f), Vector2f(1024.0f, 1024.0f), 1, 1, true, false, false);

	Globals::shapeManager.buildSphere("sphere_cl", 1.0f, Vector3f(0.0f, 8.0f, 0.0f), 32, 32, false, true, false);
	Globals::shapeManager.buildSphere("sphere_portal", 0.2f, Vector3f(512.0f, 13.75f, 544.0f), 16, 16, false, true, false);	
	Globals::fontManager.loadCharacterSet("upheaval_200", "res/fonts/upheavtt.ttf", 200, 0, 30, 128, 0, true, 0u);
	Globals::fontManager.loadCharacterSet("upheaval_50", "res/fonts/upheavtt.ttf", 50, 0, 3, 0, 0, true, 0u);


	MusicBuffer::Init();
	//SoundBuffer::Init();

	Globals::musicManager.createMusicBuffer("background");
	Globals::musicManager.get("background").setVolume(Globals::musicVolume);
	Globals::musicManager.get("background").setLooping(true);

	Globals::soundManager.createSoundBuffer("menu", 0u, 2u, Globals::soundVolume);
	Globals::soundManager.get("menu").loadChannel("res/sounds/button.wav", 0u);

	Globals::soundManager.createSoundBuffer("game", 0u, 8u, Globals::soundVolume);
	Globals::soundManager.get("game").loadChannel("res/sounds/warp.wav", 0u);
	Globals::soundManager.get("game").loadChannel("res/sounds/bounce.wav", 1u);
	Globals::soundManager.get("game").loadChannel("res/sounds/swish.wav", 2u);

	Globals::soundManager.get("game").loadChannel("res/sounds/energyflow.wav", 3u);
	Globals::soundManager.get("game").loadChannel("res/sounds/pickup.wav", 4u);
	Globals::soundManager.get("game").loadChannel("res/sounds/screwgravity.wav", 5u);

	Globals::soundManager.get("game").loadChannel("res/sounds/unlock.wav", 6u);
	Globals::soundManager.get("game").loadChannel("res/sounds/warp.wav", 7u);
}