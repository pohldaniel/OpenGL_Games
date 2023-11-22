#include <iostream>
#include "Application.h"

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	initWindow();
	initOpenGL();

	loadAssets();
	initStates();
	
	m_enableVerticalSync = true;
	m_enableWireframe = false;	
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

	m_window = CreateWindowEx(
		NULL,								
		"WINDOWCLASS",					
		"Isometric",				
		WS_OVERLAPPEDWINDOW^(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,									
		WIDTH,
		HEIGHT,									
		NULL,								
		NULL,						
		windowClass.hInstance,
		this);								

	if (!m_window)
		return;

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
		}

		default:{
			application = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			break;
		}
	}

	return application ? application->DisplayWndProc(hWnd, message, wParam, lParam) : DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Application::DisplayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	switch (message) {

		case WM_LBUTTONDOWN: { 
			//setCursortoMiddle(hWnd);
			//SetCapture(hWnd);
			break;
		}case WM_DESTROY: {
			PostQuitMessage(0);	
			break;
		}case WM_KEYDOWN: {

			switch (wParam) {
				case VK_ESCAPE: {					
					/*if (m_machine->m_states.top()->m_currentState == CurrentState::MENU) {
						PostQuitMessage(0);
					}*/
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
			Globals::projection = Matrix4f::GetOrthographic(Globals::projection, 0.f, static_cast<float>(_width), 0.0f, static_cast<float>(_height), -1.0f, 1.0f);			
			Globals::invProjection = Matrix4f::GetInvOrthographic(Globals::projection, 0.f, static_cast<float>(_width), 0.0f, static_cast<float>(_height), -1.0f, 1.0f);

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



void Application::setCursortoMiddle(HWND hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	SetCursorPos(rect.right / 2, rect.bottom / 2);
}

HWND Application::getWindow() {
	return m_window;
}

bool Application::isRunning(){
	processInput();
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {		
		if (msg.message == WM_QUIT) return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
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
	//m_machine->addStateAtTop(new Game(*m_machine));
	//m_machine->addStateAtTop(new Menu(*m_machine));
}

void Application::processInput() {
	Globals::CONTROLLS = 0;

	// Retrieve keyboard state
	if (!GetKeyboardState(Globals::pKeyBuffer)) return;
	// Check the relevant keys
	bool holdKey = ((Globals::pKeyBuffer[VK_ESCAPE] & 0xF0) && (Globals::CONTROLLSHOLD & Globals::KEY_ESCAPE)) || (Globals::pKeyBuffer['Q'] & 0xF0) && (Globals::CONTROLLSHOLD & Globals::KEY_Q);

	if (!holdKey) {
		if (Globals::pKeyBuffer['W'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_W;
		if (Globals::pKeyBuffer['A'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_A;
		if (Globals::pKeyBuffer['D'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_D;
		if (Globals::pKeyBuffer['S'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_S;
		if (Globals::pKeyBuffer['Q'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_Q;
		if (Globals::pKeyBuffer[VK_SPACE] & 0xF0) Globals::CONTROLLS |= Globals::KEY_SPACE;
		if (Globals::pKeyBuffer[VK_ESCAPE] & 0xF0) Globals::CONTROLLS |= Globals::KEY_ESCAPE;
		if (Globals::pKeyBuffer[VK_RETURN] & 0xF0) Globals::CONTROLLS |= Globals::KEY_RETURN;
		if (Globals::pKeyBuffer[VK_UP] & 0xF0) Globals::CONTROLLS |= Globals::KEY_UP;
		if (Globals::pKeyBuffer[VK_DOWN] & 0xF0) Globals::CONTROLLS |= Globals::KEY_DOWN;

		Globals::CONTROLLSHOLD = Globals::CONTROLLS;
	}

	GetCursorPos(&m_cursorPosScreenSpace);
	ScreenToClient(m_window, &m_cursorPosScreenSpace);
	Globals::cursorPosScreen = { m_cursorPosScreenSpace.x, m_cursorPosScreenSpace.y};
	Globals::cursorPosNDC = { (2.0f * m_cursorPosScreenSpace.x) / (float)WIDTH - 1.0f, 1.0f - (2.0f * m_cursorPosScreenSpace.y) / (float)HEIGHT, 0.0f};

	//near
	m_cursorPosEye = Globals::invProjection * Vector4f(Globals::cursorPosNDC.x, Globals::cursorPosNDC.y, -1.0f, 1.0f);
	Globals::cursorPosEye = { m_cursorPosEye[0] , m_cursorPosEye[1] , m_cursorPosEye[2] };

	//far
	//m_cursorPosEye = Globals::invProjection * Vector4f(Globals::cursorPosNDC.x, Globals::cursorPosNDC.y, 1.0f, 1.0f);
	Globals::lMouseButton = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
}

void Application::loadAssets() {
	Globals::shaderManager.loadShader("quad", "res/shader/quad.vs", "res/shader/quad.fs");
	Globals::shaderManager.loadShader("quad_array", "res/shader/quad_array.vs", "res/shader/quad_array.fs");


	Globals::spritesheetManager.loadSpritesheet("tiles", "res/isotiles.png", 64, 80, 0, 0, 0, -1);
	Globals::spritesheetManager.loadSpritesheet("tiles2", "res/tiles.png", 96, 48, 0, 0, 0, -1);

	Globals::textureManager.loadTexture("tile", "res/tile.png");
}