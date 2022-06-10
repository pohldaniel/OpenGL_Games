#include <iostream>
#include "Application.h"

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt), m_eventDispatcher(new EventDispatcher()) {
	initWindow();
	initOpenGL();

	loadAssets();
	initStates();
	
	m_enableVerticalSync = true;
	m_enableWireframe = false;

	Mouse::instance().smoothMouse(true);

	m_eventDispatcher->setProcessOSEvents([&]() {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	});
}

Application::~Application() {
	delete m_machine;
	
	Globals::shaderManager.clear();
	Globals::spritesheetManager.clear();

	//release OpenGL context
	HDC hdc = GetDC(m_window);
	wglMakeCurrent(GetDC(m_window), 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(m_window, hdc);
	//release OpenAL context
	//SoundDevice::shutDown();

	UnregisterClass("WINDOWCLASS", (HINSTANCE)GetModuleHandle(NULL));
}

bool Application::initWindow() {

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
		return false;

	m_window = CreateWindowEx(
		NULL,								
		"WINDOWCLASS",					
		"Deathjump",				
		WS_OVERLAPPEDWINDOW^(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,									
		WIDTH,
		HEIGHT,									
		NULL,								
		NULL,						
		windowClass.hInstance,
		this);								

	if (!m_window)
		return false;

	// Adjust it so the client area is RESOLUTION_X/RESOLUTION_Y
	RECT rect1;
	GetWindowRect(m_window, &rect1);
	RECT rect2;
	GetClientRect(m_window, &rect2);

	SetWindowPos(m_window, NULL, rect1.left, rect1.top, WIDTH + ((rect1.right - rect1.left) - (rect2.right - rect2.left)), HEIGHT + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)), NULL);

	ShowWindow(m_window, SW_SHOW);
	UpdateWindow(m_window);

	m_init = true;
}

LRESULT CALLBACK Application::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	Application* application = nullptr;

	switch (message){
		case WM_CREATE:{			
			application = static_cast<Application*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));
			break;
		}default:{
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
		case WM_KEYDOWN: {

			switch (wParam) {
				case 'v': case 'V': {
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
			Globals::projection = Matrix4f::GetOrthographic(Globals::projection, 0.0f, static_cast<float>(_width), 0.0f, static_cast<float>(_height), -1.0f, 1.0f);			
			Globals::invProjection = Matrix4f::GetInvOrthographic(Globals::invProjection, 0.f, static_cast<float>(_width), 0.0f, static_cast<float>(_height), -1.0f, 1.0f);

			if(m_init) 
				m_machine->resize(_width, _height);
				//todo resizeable ui elements
				//m_machine->m_states.top()->resize(_width, _height)
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
	glDepthFunc(GL_LEQUAL);

	//alpha test for cutting border of the quads
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);

	//button transparency, fog and light
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//https://stackoverflow.com/questions/2171085/opengl-blending-with-previous-contents-of-framebuffer
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//button outline
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
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
	}else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

HWND Application::getWindow() {
	return m_window;
}

bool Application::isRunning() {

	Keyboard::instance().update();
	Mouse::instance().update();
	if (Keyboard::instance().keyDown(Keyboard::KEY_ESCAPE)) {
		return false;
	}

	return m_eventDispatcher->update();
}

void Application::render() {
	m_machine->render();
}

void Application::update() {
	m_machine->update();	
}

void Application::fixedUpdate() {
	m_machine->fixedUpdate();
}

void Application::initStates() {
	m_machine = new StateMachine(m_dt, m_fdt);
	Game* game = dynamic_cast<Game*>(m_machine->addStateAtTop(new Game(*m_machine)));

	AddMouseListener(game);
	AddKeyboardListener(game);
}

void Application::processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message){
		case WM_CLOSE: case WM_QUIT: {
			Event event;
			event.type = Event::CLOSED;
			m_eventDispatcher->pushEvent(event);
			break;
		}case WM_MOUSEMOVE: {
			Event event;
			event.type = Event::MOUSEMOTION;
			event.mouseMove.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.mouseMove.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			m_eventDispatcher->pushEvent(event);
			if (!m_mouseTracking) {
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
				trackMouseEvent.dwFlags = TME_LEAVE;
				trackMouseEvent.hwndTrack = hWnd;
				TrackMouseEvent(&trackMouseEvent);
				m_mouseTracking = true;
			}
			break;
		}case WM_MOUSELEAVE: {
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
		}case WM_KEYDOWN: {
			Event event;
			event.type = Event::KEYDOWN;
			m_eventDispatcher->pushEvent(event);
			break;
		}case WM_KEYUP: {
			Event event;
			event.type = Event::KEYUP;
			m_eventDispatcher->pushEvent(event);
			break;
		}case WM_INPUT: {
			Mouse::instance().handleMsg(hWnd, message, wParam, lParam);
			break;
		}
	}
}

void Application::loadAssets() {
	Globals::shaderManager.loadShader("quad", "res/shader/quad.vs", "res/shader/quad.fs");
	Globals::shaderManager.loadShader("quad_color", "res/shader/quad_color.vs", "res/shader/quad_color.fs");
	Globals::shaderManager.loadShader("quad_color_uniform", "res/shader/quad_color_uniform.vs", "res/shader/quad_color_uniform.fs");
	Globals::shaderManager.loadShader("quad_array", "res/shader/quad_array.vs", "res/shader/quad_array.fs");
	Globals::shaderManager.loadShader("level", "res/shader/level.vs", "res/shader/level.fs");

	Globals::spritesheetManager.loadSpritesheet("tiles", "res/img/tiles.png", 96, 48, 0, 0, 0, -1);

	Globals::spritesheetManager.loadSpritesheet("structures", "res/img/structures.png", 288, 163, 0, 0, 0, 3);

	Globals::textureManager.loadTexture("structures", "res/img/structures.png", 0, 0, 288, 163);
	Globals::spritesheetManager.loadSpritesheet("structures", (&Globals::textureManager.get("structures"))->getTexture(), 288, 163);

	Globals::textureManager.loadTexture("structures", "res/img/structures.png", 288, 0, 288, 163);
	Globals::spritesheetManager.getAssetPointer("structures")->addToSpritesheet((&Globals::textureManager.get("structures"))->getTexture(), 288, 163);
}

void Application::AddMouseListener(MouseEventListener * el) {
	m_eventDispatcher->AddMouseListener(el);
}

void Application::AddKeyboardListener(KeyboardEventListener * el) {
	m_eventDispatcher->AddKeyboardListener(el);
}