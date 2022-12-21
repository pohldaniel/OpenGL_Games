#include <iostream>
#include "Application.h"
#include "GL.H"
#include "MainMenu.h"
#include "Game.h"
#include "Editor.h"
#include "LoadingScreen.h"
#include "ChooseClassMenu.h"
#include "Options.h"

EventDispatcher& Application::s_eventDispatcher = EventDispatcher::Get();
StateMachine* Application::s_machine = nullptr;
HGLRC Application::MainContext;
HGLRC Application::LoaderContext;
HWND Application::Window;
unsigned int Application::Width;
unsigned int Application::Height;
bool Application::Fullscreen;
bool Application::Init = false;
DWORD Application::SavedExStyle;
DWORD Application::SavedStyle;

bool compareDeviceMode(DEVMODE const& s1, DEVMODE const& s2) {
	return s1.dmPelsWidth == s2.dmPelsWidth && s1.dmPelsHeight == s2.dmPelsHeight && s1.dmDefaultSource == s2.dmDefaultSource && s1.dmDisplayFrequency == s2.dmDisplayFrequency;
}

bool findDeviceMode(DEVMODE const& s1, unsigned int width, unsigned int height) {
	return s1.dmPelsWidth == width && s1.dmPelsHeight == height && s1.dmDefaultSource == 0 && s1.dmDisplayFrequency == 120;
}

auto sortDeviceMode = [](DEVMODE const& s1, DEVMODE const& s2) -> bool {
	return s1.dmPelsWidth == s2.dmPelsWidth ? s1.dmPelsHeight < s2.dmPelsHeight : s1.dmPelsWidth < s2.dmPelsWidth;
};

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	LuaFunctions::executeLuaFile("res/_lua/save/settings.lua");

	Fullscreen = Globals::applyDisplaymode;
	Width = !Fullscreen ? 1024u : Globals::width;
	Height = !Fullscreen ? 768u : Globals::height;

	int width = Globals::width;
	int height = Globals::height;
	
	initWindow();
	ViewPort::Get().init(Width, Height);
	initOpenGL();
	initOpenAL();
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

	Globals::soundManager.get("effect").setVolume(Globals::soundVolume);
	Globals::soundManager.get("player").setVolume(Globals::soundVolume);
	Globals::musicManager.get("background").setVolume(Globals::musicVolume);

	SavedExStyle = GetWindowLong(Window, GWL_EXSTYLE);
	SavedStyle = GetWindowLong(Window, GWL_STYLE);

	if (Fullscreen)
		Application::SetDisplayMode(width, height);
}

Application::~Application() {
	WriteConfigurationToFile();
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
		Width,
		Height,
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

	SetWindowPos(Window, NULL, rect1.left, rect1.top, Width + ((rect1.right - rect1.left) - (rect2.right - rect2.left)), Height + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)), NULL);

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	Init = true;

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
			
			int deltaW = Width;
			int deltaH = Height;

			Height = HIWORD(lParam);		// retrieve width and height
			Width = LOWORD(lParam);

			deltaW = Width - deltaW;
			deltaH = Height - deltaH;

			if (Height == 0) {					// avoid divide by zero
				Height = 1;
			}
			
			resize(deltaW, deltaH);
			break;
		}case WM_GETMINMAXINFO:{			
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			//logwindow + actionbar width
			lpMMI->ptMinTrackSize.x = 1000 + 16;
			//inventory height
			lpMMI->ptMinTrackSize.y = 670 + 39;
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
			ToggleFullScreen(!Fullscreen, Fullscreen ? 1024u : 0u, Fullscreen ? 768u : 0u);
		}
	}

	if (Keyboard::instance().keyDown(Keyboard::KEY_2)) {
		ChangeDisplaySettings(NULL, 0);
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

	//s_machine->addStateAtTop(new MainMenu(*s_machine));
	s_machine->addStateAtTop(new Options(*s_machine));
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
	//Globals::soundEffectsPlayer.loadSoundEffect("effect_2", "res/sound/arrowHit06.ogg");

	Globals::musicManager.initMusicBuffer("background");
	Globals::musicManager.initMusicBuffer("foreground");

	Globals::soundManager.initSoundBuffer("player");
	Globals::soundManager.initSoundBuffer("effect");

	Globals::musicManager.get("background").setVolume(Globals::musicVolume);
	Globals::soundManager.get("effect").setVolume(Globals::soundVolume);
	Globals::soundManager.get("player").setVolume(Globals::soundVolume);
	Globals::musicManager.get("background").setLooping(true);
}


void Application::resize(int deltaW, int deltaH) {
	if (deltaW == 0 && deltaH == 0) return;

	glViewport(0, 0, Width, Height);
	Globals::projection = Matrix4f::GetPerspective(Globals::projection, 45.0f, static_cast<float>(Width) / static_cast<float>(Height), 1.0f, 5000.0f);
	Globals::invProjection = Matrix4f::GetInvPerspective(Globals::invProjection, 45.0f, static_cast<float>(Width) / static_cast<float>(Height), 1.0f, 5000.0f);
	Globals::orthographic = Matrix4f::GetOrthographic(Globals::orthographic, 0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f);
	Globals::invOrthographic = Matrix4f::GetInvOrthographic(Globals::invOrthographic, 0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f);

	if (Init) {
		ViewPort::Get().init(Width, Height);
		s_machine->resize(Width, Height);
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

void Application::GetScreenMode(std::vector<DEVMODE>& list) {
	DEVMODE screen;
	screen.dmSize = sizeof(DEVMODE);
	for (int i = 0; EnumDisplaySettings(NULL, i, &screen); i++) {
		if(screen.dmDisplayFrequency == 60) continue;
		if(screen.dmDefaultSource != 0) continue;
		if (screen.dmPelsWidth < 1024) continue;
		if (static_cast<float>(screen.dmPelsHeight) / static_cast<float>(screen.dmPelsWidth) != 0.75f) continue;
		list.push_back(screen);
	}

	DEVMODE curScreen;
	memset(&curScreen, 0, sizeof(curScreen));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curScreen);

	std::vector<DEVMODE>::iterator it = std::find_if(list.begin(), list.end(), std::bind(compareDeviceMode, std::placeholders::_1, curScreen));
	if(it == list.end()) {
		list.push_back(curScreen);
		std::sort(list.begin(), list.end(), sortDeviceMode);
	};		
}

void Application::SetDisplayMode(DEVMODE& settings) {
	DEVMODE curScreen;
	memset(&curScreen, 0, sizeof(curScreen));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curScreen);

	if(!compareDeviceMode(settings, curScreen))
		if(!ChangeDisplaySettings(&settings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
			std::cout << "Could Not Apply Display Settings: " << std::endl;

	ToggleFullScreen(true);	
}

void Application::SetDisplayMode(unsigned int width, unsigned int height) {
	std::vector<DEVMODE> list;
	GetScreenMode(list);

	std::vector<DEVMODE>::iterator it = std::find_if(list.begin(), list.end(), std::bind(&findDeviceMode, std::placeholders::_1, width, height));
	if (it != list.end()) {
		SetDisplayMode(*it);
	} else {
		Fullscreen = false;
	}
}

void Application::ResetDisplayMode() {
	ChangeDisplaySettings(NULL, 0);
	ToggleFullScreen(false, 1024u, 768u);

	Globals::width = 1024u;
	Globals::height = 768u;
}

void Application::ToggleFullScreen(bool isFullScreen, unsigned int width, unsigned int height) {
	
	static RECT rcSaved;

	int deltaW = width == 0u ? Width : width;
	int deltaH = height == 0u ? Height : height;

	if (isFullScreen) {
		if (!Fullscreen) {
			
			GetWindowRect(Window, &rcSaved);
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
		resize(deltaW, deltaH);
	}
	
	if (!isFullScreen) {
		Fullscreen = false;
		SetWindowLong(Window, GWL_EXSTYLE, SavedExStyle);
		SetWindowLong(Window, GWL_STYLE, SavedStyle);
		SetWindowPos(Window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		Width = width == 0u ? rcSaved.right - rcSaved.left : width;
		Height = height == 0u ? rcSaved.bottom - rcSaved.top : height;

		deltaW = Width - deltaW;
		deltaH = Height - deltaH;

		SetWindowPos(Window, HWND_NOTOPMOST, rcSaved.left, rcSaved.top, Width, Height, SWP_SHOWWINDOW);
		resize(deltaW, deltaH);
	}
}

void Application::WriteConfigurationToFile() {
	std::ofstream oss("res/_lua/save/settings.lua");
	oss << "setfenv(1, Globals);" << std::endl;
	oss << "setResolution(" << Globals::width << ", " << Globals::height << ");" << std::endl;
	oss << "useDisplaymode(" << std::boolalpha << Globals::applyDisplaymode << ");" << std::endl;
	oss << "setSoundVolume(" << Globals::soundVolume << ");" << std::endl;
	oss << "setMusicVolume(" << Globals::soundVolume << ");" << std::endl;
}