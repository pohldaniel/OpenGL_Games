#include <iostream>
#include "Application.h"

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	initWindow();
	initOpenGL();
	initOpenAL();
	loadAssets();
	initStates();
	
	m_enableVerticalSync = true;
	m_enableWireframe = false;	

	Transition::get().init();
}

Application::~Application() {

	delete m_machine;
	Globals::shaderManager.clear();
	Globals::textureManager.clear();
	Globals::fontManagerOld.clear();
	Globals::fontManager.clear();
	Globals::soundManager.clear();
	Globals::musicManager.clear();
	AssetManagerStatic<Texture>::get().clear();	

	UnregisterClass("WINDOWCLASS", (HINSTANCE)GetModuleHandle(NULL));

	//release OpenGL context
	wglMakeCurrent(GetDC(m_window), 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(m_window, GetDC(m_window));
	//release OpenAL context
	SoundDevice::ShutDown();

	SaveFile::save("res/save");
}

bool Application::initWindow() {

	WNDCLASSEX		windowClass;		

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
			Globals::projection = Matrix4f::GetOrthographic(Globals::projection, 0.f, static_cast<float>(_width), 0.0f, static_cast<float>(_height), -1.0f, 1.0f);
			Globals::invProjection = Matrix4f::GetInvOrthographic(Globals::invProjection, 0.f, static_cast<float>(_width), 0.0f, static_cast<float>(_height), -1.0f, 1.0f);
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
		24,								// 16 bit z-buffer size
		8,								// no stencil buffer
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void Application::initOpenAL(){
	SoundDevice::Init();
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
	Transition::get().update(m_dt);
}

void Application::fixedUpdate() {
	m_machine->fixedUpdate();
}

void Application::initStates() {
	m_machine = new StateMachine(m_dt, m_fdt);
	//m_machine->addStateAtTop(new Game(*m_machine));
	//m_machine->addStateAtTop(new Pause(*m_machine));
	m_machine->addStateAtTop(new Menu(*m_machine));
	//m_machine->addStateAtTop(new Settings(*m_machine));
}

void Application::processInput() {
	Globals::CONTROLLS = 0;

	// Retrieve keyboard state
	if (!GetKeyboardState(Globals::pKeyBuffer)) return;
	// Check the relevant keys
	bool holdKey =  ((Globals::pKeyBuffer['Q'] & 0xF0) && (Globals::CONTROLLSHOLD & Globals::KEY_Q) || (Globals::pKeyBuffer['E'] & 0xF0) && (Globals::CONTROLLSHOLD & Globals::KEY_E)) ;

	if (!holdKey) {	
		if (Globals::pKeyBuffer['Q'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_Q;
		if (Globals::pKeyBuffer['W'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_W;
		if (Globals::pKeyBuffer['E'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_E;
		if (Globals::pKeyBuffer['A'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_A;
		if (Globals::pKeyBuffer['D'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_D;
		if (Globals::pKeyBuffer['S'] & 0xF0) Globals::CONTROLLS |= Globals::KEY_S;
		if (Globals::pKeyBuffer[VK_SPACE] & 0xF0) Globals::CONTROLLS |= Globals::KEY_SPACE;

		Globals::CONTROLLSHOLD = Globals::CONTROLLS;	
	}

	if (Globals::CONTROLLS & Globals::KEY_E) {
		std::cout << "-------" << std::endl;
	}

	GetCursorPos(&m_cursorPosScreenSpace);
	ScreenToClient(m_window, &m_cursorPosScreenSpace);
	Globals::cursorPosScreen = { m_cursorPosScreenSpace.x, m_cursorPosScreenSpace.y};

	//std::cout << Globals::cursorPosScreen.x << "  " << Globals::cursorPosScreen.y <<  std::endl;

	Globals::cursorPosNDC = { (2.0f * m_cursorPosScreenSpace.x) / (float)WIDTH - 1.0f, 1.0f - (2.0f * m_cursorPosScreenSpace.y) / (float)HEIGHT, 0.0f};
	//std::cout << Globals::cursorPosNDC.x << "  " << Globals::cursorPosNDC.y << "  " << Globals::cursorPosNDC.z << std::endl;

	//near
	m_cursorPosEye = Globals::invProjection * Vector4f(Globals::cursorPosNDC.x, Globals::cursorPosNDC.y, -1.0f, 1.0f);
	Globals::cursorPosEye = { m_cursorPosEye[0] , m_cursorPosEye[1] , m_cursorPosEye[2] };
	//far
	/*m_cursorPosEye = Globals::invProjection * Vector4f(Globals::cursorPosNDC.x, Globals::cursorPosNDC.y, 1.0f, 1.0f);
	
	std::cout << Globals::cursorPosEye.x << "  " << Globals::cursorPosEye.y << "  " << Globals::cursorPosEye.z << std::endl;
	std::cout << "-------" << std::endl;*/

	Globals::lMouseButton = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
}

void Application::loadAssets() {
	
	Globals::textureManager.loadTexture("player", "res/textures/player.png");
	//Globals::textureManager.loadTexture("background", "res/textures/background.png");
	Globals::textureManager.loadTexture("foreground", "res/textures/map.png");
	Globals::textureManager.loadTexture("menu", "res/textures/menu.png");	
	AssetManagerStatic<Texture>::get().loadTexture("background", "res/textures/background.png");
	Globals::textureManager.loadTexture("fireball", "res/textures/fireball.png");

	Globals::spritesheetManager.loadSpritesheet("blow_up", "res/textures/explo.png", 512, 512, 0, 11);
	Globals::spritesheetManager.loadSpritesheet("ghost", "res/textures/ghost.png", 64, 64, 0, 3);

	//becarful with the uniforms some shader are used at multiple places
	Globals::shaderManager.loadShader("fog", "shader/fog.vs", "shader/fog.fs");
	Globals::shaderManager.loadShader("quad", "shader/quad.vs", "shader/quad.fs");
	Globals::shaderManager.loadShader("quad_color", "shader/quad_color.vs", "shader/quad_color.fs");
	Globals::shaderManager.loadShader("quad_color_single", "shader/quad_color_single.vs", "shader/quad_color_single.fs");
	Globals::shaderManager.loadShader("quad_array", "shader/quad_array.vs", "shader/quad_array.fs");
	Globals::shaderManager.loadShader("light", "shader/light.vs", "shader/light.fs");
	Globals::shaderManager.loadShader("transition", "shader/transition.vs", "shader/transition.fs");
	Globals::shaderManager.loadShader("blur", "shader/blur.vs", "shader/blur.fs");
	Globals::shaderManager.loadShader("text", "shader/text.vs", "shader/text.fs");

	Globals::fontManagerOld.loadCharacterSet("font_90", "res/fonts/upheavtt.ttf", 90.0f);
	Globals::fontManager.loadCharacterSet("font_90", "res/fonts/upheavtt.ttf", 90.0f);


	Globals::soundManager.loadSoundEffect("blowup", "res/sounds/blowup.wav");
	Globals::soundManager.loadSoundEffect("ghost", "res/sounds/ghost.wav");
	Globals::soundManager.loadSoundEffect("button", "res/sounds/button.wav");
	Globals::soundManager.loadSoundEffect("3", "res/sounds/3.wav");
	Globals::soundManager.loadSoundEffect("2", "res/sounds/2.wav");
	Globals::soundManager.loadSoundEffect("1", "res/sounds/1.wav");
	Globals::soundManager.loadSoundEffect("go", "res/sounds/go.wav");

	Globals::musicManager.loadMusic("main", "res/music/main.ogg");
	Globals::musicManager.loadMusic("menu", "res/music/menu.ogg");
	Globals::musicManager.loadMusic("pause", "res/music/pause.ogg");

	Globals::effectsPlayer.init();

	SaveFile::load("res/save");
}