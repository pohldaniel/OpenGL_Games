#include <iostream>
#include "Application.h"

#include "MainMenu.h"
#include "Game.h"
#include "Editor.h"
#include "LoadingScreen.h"

EventDispatcher& Application::s_eventDispatcher = EventDispatcher::Get();


Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	ViewPort::get().init(WIDTH, HEIGHT);
	
	initWindow();
	initOpenGL();
	loadAssets();
	initStates();
	m_enableVerticalSync = true;

	Application::s_eventDispatcher.setProcessOSEvents([&]() {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	});

	Fontrenderer::Get().init();
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font"));
	Fontrenderer::Get().setCamera(ViewPort::get().getCamera());
	Batchrenderer::Get().init(400, true);
	Batchrenderer::Get().setCamera(ViewPort::get().getCamera());	
	Batchrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("batch"));

	Instancedrenderer::Get().init();
	Instancedrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("instanced"));

	auto shader = Globals::shaderManager.getAssetPointer("batch_font");

	glUseProgram(shader->m_program);
	shader->loadInt("u_sprite", 0);
	shader->loadInt("u_font", 1);
	glUseProgram(0);
}

Application::~Application() {

	Batchrenderer::Get().shutdown();
	Instancedrenderer::Get().shutdown();
	Fontrenderer::Get().shutdown();

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
		"Dawn",
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

	// Adjust it so the client area is RESOLUTION_X/RESOLUTION_Y
	RECT rect1;
	GetWindowRect(m_window, &rect1);
	RECT rect2;
	GetClientRect(m_window, &rect2);

	SetWindowPos(m_window, NULL, rect1.left, rect1.top, WIDTH + ((rect1.right - rect1.left) - (rect2.right - rect2.left)), HEIGHT + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)), NULL);

	ShowWindow(m_window, SW_SHOW);
	UpdateWindow(m_window);

	
	m_init = true;

	return true;
}

LRESULT CALLBACK Application::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	Application* application = nullptr;

	switch (message) {
		case WM_CREATE: {
			application = static_cast<Application*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);			
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));
			break;
		}case WM_SETCURSOR: {
			if (LOWORD(lParam) == HTCLIENT) {
				SetCursor(Mouse::GetCursorIcon());
				return TRUE;
			}
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
			Mouse::instance().attach2(hWnd);
			Keyboard::instance().enable();
			break;
		}case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}case WM_KEYDOWN: {
			switch (wParam) {
				case 'V': {
					enableVerticalSync(!m_enableVerticalSync);
					break;
				}case 'Z': {
					m_machine->toggleWireframe();
					break;
				}case VK_SPACE: {
					Mouse::instance().detach2();
					Keyboard::instance().disable();
					break;
				}
			}
			break;
		}case WM_SIZE: {
			
			int deltaW = m_width;
			int deltaH = m_height;

			m_height = HIWORD(lParam);		// retrieve width and height
			m_width = LOWORD(lParam);

			deltaW = m_width - deltaW;
			deltaH = m_height - deltaH;

			if (m_height == 0) {					// avoid divide by zero
				m_height = 1;
			}
			
			glViewport(0, 0, m_width, m_height);
			Globals::projection = Matrix4f::GetPerspective(Globals::projection, 45.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 1.0f, 5000.0f);
			Globals::invProjection = Matrix4f::GetInvPerspective(Globals::invProjection, 45.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 1.0f, 5000.0f);
			Globals::orthographic = Matrix4f::GetOrthographic(Globals::orthographic, 0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height), -1.0f, 1.0f);
			Globals::invOrthographic = Matrix4f::GetInvOrthographic(Globals::invOrthographic, 0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height), -1.0f, 1.0f);
						
			if (m_init) {
				ViewPort::get().init(m_width, m_height);
				m_machine->resize(m_width, m_height);
				m_machine->m_states.top()->resize(deltaW, deltaH);
			}
			
			break;
		}case WM_GETMINMAXINFO:{			
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 1024 + 16;
			lpMMI->ptMinTrackSize.y = 768 + 39;
			break;
		}default: {
			Mouse::instance().handleMsg(hWnd, message, wParam, lParam);
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

	//glAlphaFunc(GL_NEVER, 0.0f);

	glDepthFunc(GL_ALWAYS);
	//glDepthFunc(GL_LESS);
	//glDepthFunc(GL_NEVER);
	//alpha test for cutting border of the quads
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_LEQUAL, 0.81f);

	//glAlphaFunc(GL_GEQUAL, 0.5);

	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDisable(GL_ALPHA_TEST);
	//button transparency, fog and light
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//https://stackoverflow.com/questions/2171085/opengl-blending-with-previous-contents-of-framebuffer
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);
	// enable blending

	//outline
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glEnable(GL_CLIP_DISTANCE0);
	
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);
}

void Application::enableVerticalSync(bool enableVerticalSync) {

	// WGL_EXT_swap_control.
	typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(GLint);
	static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));

	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(enableVerticalSync ? 1 : 0);
		m_enableVerticalSync = enableVerticalSync;
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
	
	return s_eventDispatcher.update();
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
	m_machine->addStateAtTop(new Game(*m_machine));

	//m_machine->addStateAtTop(new MainMenu(*m_machine));
	//m_machine->addStateAtTop(new Editor(*m_machine));
	//m_machine->addStateAtTop(new LoadingScreen(*m_machine));
}

void Application::processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_MOUSEMOVE: {			
			Event event;
			event.type = Event::MOUSEMOTION;
			event.data.mouseMove.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseMove.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			s_eventDispatcher.pushEvent(event);	
			/*if (!m_mouseTracking) {
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
				trackMouseEvent.dwFlags = TME_LEAVE;
				trackMouseEvent.hwndTrack = hWnd;
				TrackMouseEvent(&trackMouseEvent);
				m_mouseTracking = true;
			}*/
			break;
		}case WM_LBUTTONDOWN: {			
			Event event;
			event.type = Event::MOUSEBUTTONDOWN;
			event.data.mouseButton.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseButton.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			event.data.mouseButton.button = Event::MouseButtonEvent::MouseButton::BUTTON_LEFT;
			s_eventDispatcher.pushEvent(event);
			break;
		}case WM_LBUTTONUP: {		
			Event event;
			event.type = Event::MOUSEBUTTONUP;
			event.data.mouseButton.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseButton.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			event.data.mouseButton.button = Event::MouseButtonEvent::MouseButton::BUTTON_LEFT;
			s_eventDispatcher.pushEvent(event);
			break;
		}/*case WM_MOUSELEAVE: {
			m_mouseTracking = false;			
			break;
		}case WM_NCMOUSEMOVE: {
			Event event;
			event.type = Event::MOUSEMOTION;
			event.data.mouseMove.titleBar = true;
			s_eventDispatcher.pushEvent(event);
			break;
		}*/
	}
}

void Application::loadAssets() {
	Globals::shaderManager.loadShader("quad", "res/shader/quad.vs", "res/shader/quad.fs");
	Globals::shaderManager.loadShader("quad_array", "res/shader/quad_array.vs", "res/shader/quad_array.fs");
	Globals::shaderManager.loadShader("batch", "res/shader/batch.vs", "res/shader/batch.fs");
	Globals::shaderManager.loadShader("font", "res/shader/batch.vs", "res/shader/font.fs");
	Globals::shaderManager.loadShader("batch_font", "res/shader/batch.vs", "res/shader/batch_font.fs");
	Globals::shaderManager.loadShader("instanced", "res/shader/instanced.vs", "res/shader/instanced.fs");
	Globals::textureManager.createNullTexture("grey", 64, 64, 128);
	Globals::spritesheetManager.createNullSpritesheet("null", 1024, 1024, 197);

	Globals::fontManager.loadCharacterSet("verdana_20", "res/verdana.ttf", 20, 3, 20);
	Globals::fontManager.loadCharacterSet("verdana_14", "res/verdana.ttf", 14, 3, 20);
	Globals::fontManager.loadCharacterSet("verdana_13", "res/verdana.ttf", 13, 3, 20);
	Globals::fontManager.loadCharacterSet("verdana_12", "res/verdana.ttf", 12, 3, 20);
	Globals::fontManager.loadCharacterSet("verdana_11", "res/verdana.ttf", 11, 3, 20);
	Globals::fontManager.loadCharacterSet("verdana_10", "res/verdana.ttf", 10, 3, 20);
	Globals::fontManager.loadCharacterSet("verdana_9", "res/verdana.ttf", 9, 3, 20);
	Globals::fontManager.loadCharacterSet("verdana_5", "res/verdana.ttf", 5, 3, 20);	
}