#include <iostream>
#include "Application.h"

Application::Application(){
	initWindow();
	initOpenGL();

	m_enableVerticalSync = true;
	m_enableWireframe = false;
}

Application::~Application() {
	UnregisterClass("WINDOWCLASS", (HINSTANCE)GetModuleHandle(NULL));
}

bool Application::initWindow() {

	WNDCLASSEX windowClass = {};
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
		return false;

	m_window = CreateWindowEx(
		NULL,
		"WINDOWCLASS",
		"Platfromer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WIDTH,
		HEIGHT,
		NULL,
		NULL,
		windowClass.hInstance,
		this);

	if (!m_window)
		return false;

	ShowWindow(m_window, SW_SHOW);
	UpdateWindow(m_window);
}

LRESULT CALLBACK Application::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	Application* application = nullptr;

	switch (message) {
	case WM_CREATE: {
		application = static_cast<Application*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));
		break;
	}

	default: {
		application = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		break;
	}
	}

	return application ? application->DisplayWndProc(hWnd, message, wParam, lParam) : DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Application::DisplayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

	case WM_CREATE: {
		POINT pt;
		RECT rect;
		GetClientRect(hWnd, &rect);
		m_oldCursorPos.x = rect.right / 2;
		m_oldCursorPos.y = rect.bottom / 2;
		pt.x = rect.right / 2;
		pt.y = rect.bottom / 2;
		//SetCursorPos(pt.x, pt.y);
		// set the cursor to the middle of the window and capture the window via "SendMessage"
		SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
		break;
	}case WM_LBUTTONDOWN: {
		//setCursortoMiddle(hWnd);
		//SetCapture(hWnd);
		break;
	}case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}case WM_KEYDOWN: {

		switch (wParam) {
		case VK_ESCAPE: {
			PostQuitMessage(0);
			break;
		}case 'v': case 'V': {
			enableVerticalSync(!m_enableVerticalSync);
			break;
		}case 'z': case 'Z': {
			enableWireframe(!m_enableWireframe);
			break;
		}
		}
		break;
	}case WM_SIZE: {

		int _height = HIWORD(lParam);		// retrieve width and height
		int _width = LOWORD(lParam);

		if (_height == 0) {					// avoid divide by zero
			_height = 1;
		}
		glViewport(0, 0, _width, _height);
		break;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Application::initOpenGL() {

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context

	hDC = GetDC(m_window);
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
		16,								// 16 bit z-buffer size
		0,								// no stencil buffer
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
	glDepthFunc(GL_LEQUAL);

	//alpha test for cutting border of the quads
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);

	//blending for tranperancy after dead see Character::render()  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void Application::enableWireframe(bool enableWireframe) {

	m_enableWireframe = enableWireframe;

	if (m_enableWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Application::setCursortoMiddle(HWND hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	SetCursorPos(rect.right / 2, rect.bottom / 2);
}

HWND Application::getWindow() {
	return m_window;
}

bool Application::isRunning() {
	processInput();
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

void Application::processInput() {
	Globals::CONTROLLS = 0;

	// Retrieve keyboard state
	if (!GetKeyboardState(Globals::pKeyBuffer)) return;
	// Check the relevant keys
	
	if (Globals::pKeyBuffer[VK_LEFT] & 0xF0) Globals::CONTROLLS |= Globals::KEY_LEFT;
	if (Globals::pKeyBuffer[VK_RIGHT] & 0xF0) Globals::CONTROLLS |= Globals::KEY_RIGHT;
	if (Globals::pKeyBuffer[VK_UP] & 0xF0) Globals::CONTROLLS |= Globals::KEY_UP;
	if (Globals::pKeyBuffer[VK_SPACE] & 0xF0) Globals::CONTROLLS |= Globals::KEY_UP;	
}