#include <iostream>
#include "Application.h"
#include "GL.H"
#include "MainMenu.h"
#include "Game.h"
#include "Editor.h"
#include "LoadingScreen.h"
#include "ChooseClassMenu.h"

EventDispatcher& Application::s_eventDispatcher = EventDispatcher::Get();
StateMachine* Application::s_machine = nullptr;
HGLRC Application::MainContext;
HGLRC Application::LoaderContext;
HWND Application::Window;

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	ViewPort::Get().init(WIDTH, HEIGHT);
	initWindow();
	initOpenGL();
	initOpenAL();
	loadAssets();
	initStates();
	m_enableVerticalSync = true;
	m_isFullScreen = false;

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
	Batchrenderer::Get().init(1200, true);
	Batchrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("batch_font"));
	Fontrenderer::Get().setRenderer(&Batchrenderer::Get());

	Instancedrenderer::Get().init();
	Instancedrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("instanced"));

	auto shader = Globals::shaderManager.getAssetPointer("batch_font");

	glUseProgram(shader->m_program);
	shader->loadInt("u_sampler[0]", 0); //font                   sampler
	shader->loadInt("u_sampler[1]", 1); //linear spell symbols   sampler
	shader->loadInt("u_sampler[2]", 2); //nearest interface      sampler
	shader->loadInt("u_sampler[3]", 3); //spells                 sampler
	shader->loadInt("u_sampler[4]", 4); //player                 sampler
	shader->loadInt("u_sampler[5]", 5); //npc                    sampler
	shader->loadInt("u_sampler[6]", 6); //zone                   sampler
	shader->loadInt("u_sampler[7]", 7); //interaction background sampler
	glUseProgram(0);

	glGenBuffers(1, &Globals::viewUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, Globals::viewUbo);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, Globals::viewBinding, Globals::viewUbo, 0, 64);

	glUniformBlockBinding(shader->m_program, glGetUniformBlockIndex(shader->m_program, "u_view"), Globals::viewBinding);
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", ViewPort::Get().getCamera().getOrthographicMatrix());
	glUseProgram(0);

	shader = Globals::shaderManager.getAssetPointer("font");
	glUniformBlockBinding(shader->m_program, glGetUniformBlockIndex(shader->m_program, "u_view"), Globals::viewBinding);
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", ViewPort::Get().getCamera().getOrthographicMatrix());
	glUseProgram(0);

	shader = Globals::shaderManager.getAssetPointer("batch");
	glUniformBlockBinding(shader->m_program, glGetUniformBlockIndex(shader->m_program, "u_view"), Globals::viewBinding);
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", ViewPort::Get().getCamera().getOrthographicMatrix());
	glUseProgram(0);

	unsigned int sampler;
	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindSampler(1, sampler);
}

Application::~Application() {

	Batchrenderer::Get().shutdown();
	Instancedrenderer::Get().shutdown();
	Fontrenderer::Get().shutdown();

	Globals::shaderManager.clear();
	Globals::spritesheetManager.clear();

	//release OpenGL context
	HDC hdc = GetDC(Window);
	wglMakeCurrent(GetDC(Window), 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(Window, hdc);
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

	Window = CreateWindowEx(
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

	if (!Window)
		return false;

	// Adjust it so the client area is RESOLUTION_X/RESOLUTION_Y
	RECT rect1;
	GetWindowRect(Window, &rect1);
	RECT rect2;
	GetClientRect(Window, &rect2);

	SetWindowPos(Window, NULL, rect1.left, rect1.top, WIDTH + ((rect1.right - rect1.left) - (rect2.right - rect2.left)), HEIGHT + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)), NULL);

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

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
	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) return 0;
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
					s_machine->toggleWireframe();
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
			
			resize(deltaW, deltaH);
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

	HDC hDC = GetDC(Window);
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
		0,								// 24 bit z-buffer size
		0,								// 8 bit stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main drawing plane
		0,								// reserved
		0, 0, 0 };						// layer masks ignored

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);	// choose best matching pixel format
	SetPixelFormat(hDC, nPixelFormat, &pfd);		// set pixel format to device context


	MainContext = wglCreateContext(hDC);				// create rendering context and make it current
	LoaderContext = wglCreateContext(hDC);
	wglShareLists(LoaderContext, MainContext);
	wglMakeCurrent(hDC, MainContext);
	ReleaseDC(Window, hDC);

	enableVerticalSync(true);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void Application::initOpenAL() {
	SoundDevice::init();
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
			toggleFullScreen();
		}
	}
	
	return s_eventDispatcher.update();
}

void Application::render() {
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &ViewPort::Get().getCamera().getViewMatrix()[0][0]);
	s_machine->render();
}

void Application::update() {
	Globals::musicManager.get("background").updateBufferStream();
	Globals::musicManager.get("foreground").updateBufferStream();
	s_machine->update();
}

void Application::fixedUpdate() {
	s_machine->fixedUpdate();
}

void Application::initStates() {
	s_machine = new StateMachine(m_dt, m_fdt);
	//s_machine->addStateAtTop(new Game(*s_machine));

	s_machine->addStateAtTop(new MainMenu(*s_machine));
	//s_machine->addStateAtTop(new Editor(*s_machine));
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

	Globals::spritesheetManager.createEmptySpritesheet("background", 128, 128);

	Globals::fontManager.loadCharacterSet("verdana_5", "res/verdana.ttf", 5, 3, 20, 128, true, 0u);
	Globals::fontManager.loadCharacterSet("verdana_9", "res/verdana.ttf", 9, 3, 20, 128, true, 1u);
	Globals::fontManager.loadCharacterSet("verdana_10", "res/verdana.ttf", 10, 3, 20, 128, true, 2u);
	Globals::fontManager.loadCharacterSet("verdana_11", "res/verdana.ttf", 11, 3, 20, 128, true, 3u);
	Globals::fontManager.loadCharacterSet("verdana_12", "res/verdana.ttf", 12, 3, 20, 128, true, 4u);
	Globals::fontManager.loadCharacterSet("verdana_13", "res/verdana.ttf", 13, 3, 20, 128, true, 5u);
	Globals::fontManager.loadCharacterSet("verdana_14", "res/verdana.ttf", 14, 3, 20, 128, true, 6u);
	Globals::fontManager.loadCharacterSet("verdana_20", "res/verdana.ttf", 20, 3, 20, 128, true, 7u);

	Globals::spritesheetManager.createSpritesheetFromTexture("font", Globals::fontManager.get("verdana_5").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("verdana_9").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("verdana_10").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("verdana_11").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("verdana_12").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("verdana_13").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("verdana_14").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->addToSpritesheet(Globals::fontManager.get("verdana_20").spriteSheet, GL_RED, GL_RED, 1);
	Globals::spritesheetManager.getAssetPointer("font")->setLinear();
	
	//Spritesheet::Safe("font", Globals::spritesheetManager.getAssetPointer("font")->getAtlas());
	
	Globals::musicManager.initMusicBuffer("background");
	Globals::musicManager.initMusicBuffer("foreground");

	Globals::soundManager.initSoundBuffer("player");
	Globals::soundManager.get("player").setVolume(0.1f);
	Globals::soundManager.initSoundBuffer("effect");
	Globals::soundManager.get("effect").setVolume(1.0f);

	Globals::soundEffectsPlayer.loadSoundEffect("effect_2", "res/sound/arrowHit06.ogg");

	
}

void Application::toggleFullScreen() {
	static DWORD savedExStyle;
	static DWORD savedStyle;
	static RECT rcSaved;

	int deltaW = m_width;
	int deltaH = m_height;

	m_isFullScreen = !m_isFullScreen;

	if (m_isFullScreen){

		savedExStyle = GetWindowLong(Window, GWL_EXSTYLE);
		savedStyle = GetWindowLong(Window, GWL_STYLE);
		GetWindowRect(Window, &rcSaved);

		SetWindowLong(Window, GWL_EXSTYLE, 0);
		SetWindowLong(Window, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		SetWindowPos(Window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		m_width = GetSystemMetrics(SM_CXSCREEN);
		m_height = GetSystemMetrics(SM_CYSCREEN);

		deltaW = m_width - deltaW;
		deltaH = m_height - deltaH;

		SetWindowPos(Window, HWND_TOPMOST, 0, 0, m_width, m_height, SWP_SHOWWINDOW);

	}else{

		SetWindowLong(Window, GWL_EXSTYLE, savedExStyle);
		SetWindowLong(Window, GWL_STYLE, savedStyle);
		SetWindowPos(Window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		m_width = rcSaved.right - rcSaved.left;
		m_height = rcSaved.bottom - rcSaved.top;

		deltaW = m_width - deltaW;
		deltaH = m_height - deltaH;

		SetWindowPos(Window, HWND_NOTOPMOST, rcSaved.left, rcSaved.top, m_width, m_height, SWP_SHOWWINDOW);
	}

	resize(deltaW, deltaH);
}

void Application::resize(int deltaW, int deltaH) {
	glViewport(0, 0, m_width, m_height);
	Globals::projection = Matrix4f::GetPerspective(Globals::projection, 45.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 1.0f, 5000.0f);
	Globals::invProjection = Matrix4f::GetInvPerspective(Globals::invProjection, 45.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 1.0f, 5000.0f);
	Globals::orthographic = Matrix4f::GetOrthographic(Globals::orthographic, 0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height), -1.0f, 1.0f);
	Globals::invOrthographic = Matrix4f::GetInvOrthographic(Globals::invOrthographic, 0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height), -1.0f, 1.0f);

	if (m_init) {
		ViewPort::Get().init(m_width, m_height);
		s_machine->resize(m_width, m_height);
		s_machine->m_states.top()->resize(deltaW, deltaH);

		auto shader = Globals::shaderManager.getAssetPointer("batch_font");

		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", ViewPort::Get().getCamera().getOrthographicMatrix());
		glUseProgram(0);

		shader = Globals::shaderManager.getAssetPointer("batch");

		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", ViewPort::Get().getCamera().getOrthographicMatrix());
		glUseProgram(0);

		shader = Globals::shaderManager.getAssetPointer("font");

		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", ViewPort::Get().getCamera().getOrthographicMatrix());
		glUseProgram(0);
	}
}

void Application::AddStateAtTop(State* state) {
	s_machine->addStateAtTop(state);
}

StateMachine& Application::GetStateMachine() {
	return *s_machine;
}