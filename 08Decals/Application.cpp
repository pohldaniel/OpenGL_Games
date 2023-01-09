#include <iostream>
#include "Application.h"
#include "engine/Framebuffer.h"

EventDispatcher& Application::EventDispatcher = EventDispatcher::Get();
StateMachine* Application::Machine = nullptr;
HWND Application::Window;
unsigned int Application::Width;
unsigned int Application::Height;
bool Application::Init = false;
bool Application::Fullscreen = false;
DWORD Application::SavedExStyle;
DWORD Application::SavedStyle;
RECT Application::Savedrc;

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt){
	Width = WIDTH;
	Height = HEIGHT;
	
	initWindow();
	initOpenGL();
	loadAssets();
	initStates();

	m_enableVerticalSync = true;

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
	Framebuffer::SetDefaultSize(Width, Height);
}

Application::~Application() {
	//release OpenGL context
	HDC hdc = GetDC(Window);
	wglMakeCurrent(GetDC(Window), 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(Window, hdc);

	UnregisterClass("WINDOWCLASS", (HINSTANCE)GetModuleHandle(NULL));
}

void Application::initWindow() {

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
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass))
		return;

	Window = CreateWindowEx(
		NULL,
		"WINDOWCLASS",
		"Voyager",
		WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,
		Width,
		Height,
		NULL,
		NULL,
		windowClass.hInstance,
		this);

	if (!Window)
		return;

	// Adjust it so the client area is RESOLUTION_X/RESOLUTION_Y
	RECT rect1;
	GetWindowRect(Window, &rect1);
	RECT rect2;
	GetClientRect(Window, &rect2);

	SetWindowPos(Window, NULL, rect1.left, rect1.top, Width + ((rect1.right - rect1.left) - (rect2.right - rect2.left)), Height + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)), NULL);

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	Init = true;
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

	if (application) {
		application->processEvent(hWnd, message, wParam, lParam);
		return application->DisplayWndProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Application::DisplayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
		case WM_CREATE: {
			//Mouse::instance().setHwnd(hWnd);
			Keyboard::instance().enable();
			break;
		}case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}case WM_LBUTTONDOWN: { // Capture the mouse			
			Mouse::instance().setAbsolute(LOWORD(lParam), HIWORD(lParam));
			break;
		}case WM_RBUTTONDOWN: {
			Mouse::instance().attach2(hWnd);
			Keyboard::instance().enable();
			break;
		}case WM_RBUTTONUP: {
			Mouse::instance().detach2();
			break;
		}case WM_KEYDOWN: {

			switch (wParam) {
				case 'v': case 'V': {
					enableVerticalSync(!m_enableVerticalSync);
					break;
				}case 'z': case 'Z': {
					Machine->toggleWireframe();
					break;
				}case VK_SPACE: {
					//  Mouse::instance().detach2();
					Keyboard::instance().disable();
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

			glViewport(0, 0, Width, Height);
			Globals::projection = Matrix4f::GetPerspective(Globals::projection, 45.0f, static_cast<float>(Width) / static_cast<float>(Height), 1.0f, 5000.0f);
			Globals::invProjection = Matrix4f::GetInvPerspective(Globals::invProjection, 45.0f, static_cast<float>(Width) / static_cast<float>(Height), 1.0f, 5000.0f);
			Globals::orthographic = Matrix4f::GetOrthographic(Globals::orthographic, 0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f);
	
			break;
		}default: {
			//Mouse::instance().handleMsg(hWnd, message, wParam, lParam);
			break;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Application::initOpenGL() {

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
	enableVerticalSync(true);

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	//glDisable(GL_CULL_FACE);
}

void Application::enableVerticalSync(bool enableVerticalSync) {

	// WGL_EXT_swap_control.
	typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(GLint);
	static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(
			wglGetProcAddress("wglSwapIntervalEXT"));

	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(enableVerticalSync ? 1 : 0);
		m_enableVerticalSync = enableVerticalSync;
	}
}

HWND Application::getWindow() {
	return Window;
}

bool Application::isRunning() {

	Keyboard::instance().update();
	Mouse::instance().update();
	if (Keyboard::instance().keyDown(Keyboard::KEY_ESCAPE)) {
		return false;
	}

	if (Keyboard::instance().keyDown(Keyboard::KEY_LALT) || Keyboard::instance().keyDown(Keyboard::KEY_RALT)) {
		if (Keyboard::instance().keyPressed(Keyboard::KEY_ENTER)) {
			ToggleFullScreen(!Fullscreen);
		}
	}

	return EventDispatcher.update();
}

void Application::render() {
	Machine->render();
}

void Application::update() {
	Machine->update();
}

void Application::fixedUpdate() {
	Machine->fixedUpdate();
}

void Application::initStates() {
	Machine = new StateMachine(m_dt, m_fdt);
	Machine->addStateAtTop(new Game(*Machine));
}

void Application::processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		/*case WM_CLOSE: case WM_QUIT: {
			Event event;
			event.type = Event::CLOSED;
			m_eventDispatcher->pushEvent(event);
			break;
		}*/case WM_MOUSEMOVE: {
			Event event;
			event.type = Event::MOUSEMOTION;
			event.data.mouseMove.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseMove.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			EventDispatcher.pushEvent(event);
			/*if (!m_mouseTracking) {
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
				trackMouseEvent.dwFlags = TME_LEAVE;
				trackMouseEvent.hwndTrack = hWnd;
				TrackMouseEvent(&trackMouseEvent);
				m_mouseTracking = true;
			}*/
			break;
		}/*case WM_MOUSELEAVE: {
			m_mouseTracking = false;

			POINT cursor;
			GetCursorPos(&cursor);
			ScreenToClient(hWnd, &cursor);

			Event event;
			event.type = Event::MOUSEMOTION;
			event.mouseMove.x = cursor.x;
			event.mouseMove.y = cursor.y;
			m_eventDispatcher->pushEvent(event);
			break;
		}*/
	}
}


void Application::Resize(int deltaW, int deltaH) {
	glViewport(0, 0, Width, Height);
	Globals::projection = Matrix4f::GetPerspective(Globals::projection, 45.0f, static_cast<float>(Width) / static_cast<float>(Height), 1.0f, 5000.0f);
	Globals::invProjection = Matrix4f::GetInvPerspective(Globals::invProjection, 45.0f, static_cast<float>(Width) / static_cast<float>(Height), 1.0f, 5000.0f);
	Globals::orthographic = Matrix4f::GetOrthographic(Globals::orthographic, 0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f);
	
	if (Init) {
		Machine->resize(Width, Height);
		Machine->m_states.top()->resize(deltaW, deltaH);
		Framebuffer::SetDefaultSize(Width, Height);
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

		Width = GetSystemMetrics(SM_CXSCREEN);
		Height = GetSystemMetrics(SM_CYSCREEN);

		deltaW = Width - deltaW;
		deltaH = Height - deltaH;

		SetWindowPos(Window, HWND_TOPMOST, 0, 0, Width, Height, SWP_SHOWWINDOW);

		Resize(deltaW, deltaH);
	}

	if (!isFullScreen) {
		Fullscreen = false;

		SetWindowLong(Window, GWL_EXSTYLE, SavedExStyle);
		SetWindowLong(Window, GWL_STYLE, SavedStyle);
		SetWindowPos(Window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		Width = width == 0u ? Savedrc.right - Savedrc.left : width;
		Height = height == 0u ? Savedrc.bottom - Savedrc.top : height;

		deltaW = Width - deltaW;
		deltaH = Height - deltaH;

		SetWindowPos(Window, HWND_NOTOPMOST, 0, 0, Width, Height, SWP_SHOWWINDOW);
		Resize(deltaW, deltaH);
	}
}

void Application::loadAssets() {

	Globals::shaderManager.loadShader("quad", "res/shader/quad.vs", "res/shader/quad.fs");
	Globals::shaderManager.loadShader("quad_shadow", "res/shader/quad_shadow.vs", "res/shader/quad_shadow.fs");
	Globals::shaderManager.loadShader("skybox", "res/shader/skybox.vs", "res/shader/skybox.fs");
	
	Globals::shaderManager.loadShader("decals", "res/shader/decals/uv_space_vs.glsl", "res/shader/decals/decal_project_fs.glsl");
	Globals::shaderManager.loadShader("texture_init", "res/shader/decals/uv_space_vs.glsl", "res/shader/decals/texture_init_fs.glsl");
	Globals::shaderManager.loadShader("depth_dec", "res/shader/decals/depth_vs.glsl", "res/shader/decals/depth_fs.glsl");
	Globals::shaderManager.loadShader("mesh", "res/shader/decals/mesh.vs", "res/shader/decals/mesh.fs");

	Globals::shaderManager.loadShader("g_buffer", "res/shader/deferred/g_buffer.vs", "res/shader/deferred/g_buffer.fs");
	Globals::shaderManager.loadShader("decals_defferred", "res/shader/deferred/decals.vs", "res/shader/deferred/decals.fs");
	Globals::shaderManager.loadShader("combiner", "res/shader/deferred/combiner.vs", "res/shader/deferred/combiner.fs");
	Globals::shaderManager.loadShader("deferred", "res/shader/deferred/deferred.vs", "res/shader/deferred/deferred.fs");

	Globals::textureManager.createNullTexture("null");
	Globals::textureManager.createNullTexture("grey", 2, 2, 128);
	Globals::textureManager.loadTexture("boots", "res/textures/Boots1.tga", true);

	Globals::textureManager.loadTexture("decal", "res/textures/Decal_04_Albedo.tga", true);
	Globals::textureManager.loadTexture("decal_n", "res/textures/Decal_04_Normal.png", true);

	Globals::textureManager.loadTexture("stone", "res/textures/Stylized_Stone_Floor_005_basecolor.jpg", true);
	Globals::textureManager.loadTexture("stone_normal", "res/textures/Stylized_Stone_Floor_005_normal.jpg", true);
	Globals::textureManager.get("stone").setLinear();
	Globals::textureManager.get("stone").setRepeat();
	Globals::textureManager.get("stone_normal").setLinear();
	Globals::textureManager.get("stone_normal").setRepeat();

	std::string facesDay[] = { "res/cubemap/day/right.png", "res/cubemap/day/left.png", "res/cubemap/day/top.png", "res/cubemap/day/bottom.png", "res/cubemap/day/back.png", "res/cubemap/day/front.png", };
	Globals::cubemapManager.loadCubeMap("day", facesDay, false);

	std::string facesNight[] = { "res/cubemap/night/right.png", "res/cubemap/night/left.png", "res/cubemap/night/top.png", "res/cubemap/night/bottom.png", "res/cubemap/night/back.png", "res/cubemap/night/front.png", };
	Globals::cubemapManager.loadCubeMap("night", facesNight, false);
}