#include <iostream>
#include <time.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#include <engine/sound/SoundDevice.h>
#include <engine/ui/Widget.h>
#include <engine/Framebuffer.h>
#include <engine/Fontrenderer.h>
#include <engine/DebugRenderer.h>
#include <engine/Sprite.h>
#include <engine/utils/Utils.h>

#include <States/Menu.h>
#include <States/Default.h>
#include <States/Tmx.h>
#include <States/MonsterHunter.h>
#include <States/Bars.h>

#include "Application.h"
#include "Globals.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EventDispatcher& Application::EventDispatcher = EventDispatcher::Get();
StateMachine* Application::Machine = nullptr;
HWND Application::Window;
int Application::Width;
int Application::Height;
bool Application::InitWindow = false;
bool Application::Fullscreen = false;
bool Application::Init = false;
DWORD Application::SavedExStyle;
DWORD Application::SavedStyle;
RECT Application::Savedrc;

HCURSOR Application::Cursor = LoadCursor(nullptr, IDC_ARROW);
//HCURSOR Application::Cursor = LoadCursor(nullptr, IDC_NO);
//HCURSOR Application::Cursor = LoadCursorFromFileA("res/cursors/rotate.cur");

HICON Application::Icon = (HICON)LoadImage(NULL, "res/icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
//HICON Application::Icon = LoadIcon(NULL, IDI_QUESTION);
//HICON Application::Icon = (HICON)LoadImage(NULL, IDI_HAND, IMAGE_ICON, 0, 0, LR_SHARED);

bool Application::VerticalSync = true;

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	Width = WIDTH;
	Height = HEIGHT;

	createWindow();
	initOpenGL(16);
	showWindow();
	initImGUI();
	initOpenAL();
	loadAssets();

	Framebuffer::SetDefaultSize(Width, Height);
	Widget::Init(Width, Height);
	Sprite::Init(Width, Height);
	ui::Widget::Init();

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

	Fontrenderer::Get().init(200, true);
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font"));

	Batchrenderer::Get().init(1000, false, false);
	Batchrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("batch"));

	DebugRenderer::Get().init(8200);
	DebugRenderer::Get().disable();

	auto shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();

	initStates();
}
Application::~Application() {
	Fontrenderer::Get().shutdown();
	DebugRenderer::Get().shutdown();
	Batchrenderer::Get().shutdown();

	delete Machine;
	Globals::shaderManager.clear();
	Widget::CleanUp();
    Sprite::CleanUp();
	Globals::soundManager.get("game").cleanup();
	Globals::musicManager.get("background").cleanup();
	SoundDevice::shutDown();

	ImGui::DestroyContext();

	HDC hdc = GetDC(Window);
	wglMakeCurrent(hdc, 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(Window, hdc);

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
	windowClass.hCursor = (HCURSOR)Cursor;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "WINDOWCLASS";
	windowClass.hIconSm = (HICON)Icon;

	if (!RegisterClassEx(&windowClass))
		return;

	Window = CreateWindowEx(
		NULL,
		"WINDOWCLASS",
		"Monster Hunter",
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

	if ((message == WM_KEYDOWN && (wParam == 'v' || wParam == 'V' || wParam == 'z' || wParam == 'Z')) || (message == WM_KEYDOWN && wParam == VK_ESCAPE) || (message == WM_KEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN))) || (message == WM_SYSKEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN)))) {
		ImGui::GetIO().WantCaptureMouse = false;
	}

	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	if (InitWindow && ImGui::GetIO().WantCaptureMouse) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	if (application) {
		application->processEvent(hWnd, message, wParam, lParam);
		return application->ApplicationWndProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Application::ApplicationWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

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
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 32,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
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

	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
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

const HWND& Application::GetWindow() {
	return Window;
}

StateMachine* Application::GetMachine() {
	return Machine;
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
	Machine->addStateAtTop(new Menu(*Machine));
	//Machine->addStateAtTop(new Tmx(*Machine));
	//Machine->addStateAtTop(new MonsterHunter(*Machine));
	//Machine->addStateAtTop(new Bars(*Machine));
}

void Application::processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_MOUSEMOVE: {
		Event event;
		event.type = Event::MOUSEMOTION;
		event.data.mouseMove.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
		event.data.mouseMove.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
		event.data.mouseMove.button = wParam & MK_RBUTTON ? Event::MouseMoveEvent::MouseButton::BUTTON_RIGHT : wParam & MK_LBUTTON ? Event::MouseMoveEvent::MouseButton::BUTTON_LEFT : Event::MouseMoveEvent::MouseButton::NONE;
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
	} case WM_KEYDOWN: {
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
		}case VK_UP: {
			Event event;
			event.type = Event::KEYDOWN;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_DOWN: {
			Event event;
			event.type = Event::KEYDOWN;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_LEFT: {
			Event event;
			event.type = Event::KEYDOWN;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_RIGHT: {
			Event event;
			event.type = Event::KEYDOWN;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_CONTROL: {

			//Alt Graph
			//if (HIWORD(lParam) == 29)
			//	break;

			Event event;
			event.type = Event::KEYDOWN;
			event.data.keyboard.keyCode = (lParam & 0x01000000) != 0 ? VK_RCONTROL : VK_LCONTROL;
			EventDispatcher.pushEvent(event);
			break;
		}
		//case VK_MENU: {
		//	Event event;
		//	event.type = Event::KEYDOWN;
		//	event.data.keyboard.keyCode = VK_RMENU;
		//	EventDispatcher.pushEvent(event);
		//	break;
		//}
		//case VK_HOME: {
		//	Event event;
		//	event.type = Event::KEYDOWN;
		//	event.data.keyboard.keyCode = VK_HOME;
		//	EventDispatcher.pushEvent(event);
		//	break;
		//}
#if DEVBUILD
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
	}case WM_KEYUP: {

		switch (wParam) {

		case VK_ESCAPE: {
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_SPACE: {
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_UP: {
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_DOWN: {
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_LEFT: {
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_RIGHT: {
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);
			break;
		}case VK_CONTROL: {			
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = (lParam & 0x01000000) != 0 ? VK_RCONTROL : VK_LCONTROL;
			EventDispatcher.pushEvent(event);
			break;
		}
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
	}case WM_SYSKEYUP: {

		switch (wParam) {
		case VK_MENU: {
			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = VK_LMENU;
			EventDispatcher.pushEvent(event);
			break;
		}
		}
		break;
	}case WM_MOUSEWHEEL: {
		Event event;
		event.type = Event::MOUSEWHEEL;
		event.data.mouseWheel.delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		event.data.mouseWheel.direction = event.data.mouseWheel.delta > 0 ? Event::MouseWheelEvent::WheelDirection::UP : Event::MouseWheelEvent::WheelDirection::DOWN;
		EventDispatcher.pushEvent(event);
		break;
	}
	}
}

void Application::Resize(int deltaW, int deltaH) {
	glViewport(0, 0, Width, Height);
	
	if (Init) {
		Framebuffer::SetDefaultSize(Width, Height);
		Widget::Resize(Width, Height);

		auto shader = Globals::shaderManager.getAssetPointer("font");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f));
		shader->unuse();

		shader = Globals::shaderManager.getAssetPointer("batch");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Width), 0.0f, static_cast<float>(Height), -1.0f, 1.0f));
		shader->unuse();

		Machine->getStates().top()->resize(deltaW, deltaH);
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

		Width = width == 0u ? GetSystemMetrics(SM_CXSCREEN) : width;
		Height = height == 0u ? GetSystemMetrics(SM_CYSCREEN) : height;

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
	SetClassLongPtr(Window, GCLP_HCURSOR, LONG_PTR(Cursor));
}

void Application::SetCursorIcon(LPCSTR resource) {
	Application::Cursor = LoadCursor(nullptr, resource);
	SetCursor(Cursor);
	SetClassLongPtr(Window, GCLP_HCURSOR, LONG_PTR(Cursor));
}

void  Application::SetCursorIcon(HCURSOR cursor) {
	Application::Cursor = cursor;
	SetCursor(Cursor);
	SetClassLongPtr(Window, GCLP_HCURSOR, LONG_PTR(Cursor));
}

void Application::loadAssets() {
	Globals::shaderManager.loadShader("font", "res/shader/batch.vert", "res/shader/font.frag");
	Globals::shaderManager.loadShader("batch", "res/shader/batch.vert", "res/shader/batch.frag");
	Globals::shaderManager.loadShader("evolve", "res/shader/evolve.vert", "res/shader/evolve.frag");
	Globals::shaderManager.loadShader("quad", "res/shader/quad.vert", "res/shader/quad.frag");
	
	Globals::fontManager.loadCharacterSet("upheaval_200", "res/fonts/upheavtt.ttf", 200, 0, 30, 128, 0, true, 0u);
	Globals::fontManager.loadCharacterSet("upheaval_50", "res/fonts/upheavtt.ttf", 30, 0, 3, 0, 0, true, 0u);
	Globals::fontManager.loadCharacterSet("dialog", "res/fonts/PixeloidSans.ttf", 400, 100, 200, 256, 0, true, 0u);
	Globals::fontManager.get("dialog").setLinearMipMap();
	//Globals::fontManager.get("dialog").safeFont("dialog");
	Globals::fontManager.loadCharacterSet("bold", "res/fonts/dogicapixelbold.otf", 400, 100, 200, 256, 0, true, 0u);
	Globals::fontManager.get("bold").setLinearMipMap();
	//Globals::fontManager.get("bold").safeFont("bold");

	Globals::textureManager.loadTexture("forest_1", "res/backgrounds/Forest/plx-1.png");
	Globals::textureManager.loadTexture("forest_2", "res/backgrounds/Forest/plx-2.png");
	Globals::textureManager.loadTexture("forest_3", "res/backgrounds/Forest/plx-3.png");
	Globals::textureManager.loadTexture("forest_4", "res/backgrounds/Forest/plx-4.png");
	Globals::textureManager.loadTexture("forest_5", "res/backgrounds/Forest/plx-5.png");

	Globals::textureManager.loadTexture("forest", "res/tmx/graphics/backgrounds/forest.png");
	Globals::textureManager.loadTexture("sand", "res/tmx/graphics/backgrounds/sand.png");
	Globals::textureManager.loadTexture("ice", "res/tmx/graphics/backgrounds/ice.png");
	Globals::textureManager.createNullTexture("null");
	Globals::spritesheetManager.createNullSpritesheet("null", 2u, 2u, 1u);

	Globals::shapeManager.buildQuadXY("quad", Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), 1, 1, true, false, false);
	Globals::shapeManager.buildQuadXY("quad_aligned", Vector3f(0.0f, 0.0f, 0.0f), Vector2f(2.0f, 2.0f), 1, 1, true, false, false);
	Globals::shapeManager.buildQuadXY("quad_half_aligned", Vector3f(0.0f, 0.0f, 0.0f), Vector2f(1.0f, 1.0f), 1, 1, true, false, false);
	
	SoundBuffer::Init();
	Globals::soundManager.createSoundBuffer("game", 5u, 20u, 0.1f);

	MusicBuffer::Init();
	Globals::musicManager.createMusicBuffer("background", 0.1f);
	Globals::musicManager.get("background").setLooping(true);
	Globals::musicManager.get("background").run();

	//Highlight post processing
	/*for (unsigned int x = 0; x < 4; x++) {
		for (unsigned int y = 0; y < 2; y++) {
			Texture::CutSubimage("res/tmx/graphics/monsters/Charmadillo.png", "Charmadillo.png", x * 192u, y * 192u, 192u, 192u);
			Texture::AddRemoveBottomPadding("Charmadillo.png", "Charmadillo_padded.png", 4);
			Texture::AddRemoveTopPadding("Charmadillo_padded.png", "Charmadillo_padded.png", 4);
			Texture::AddRemoveLeftPadding("Charmadillo_padded.png", "Charmadillo_padded.png", 4);
			Texture::AddRemoveRightPadding("Charmadillo_padded.png", "Charmadillo_padded_" + std::to_string(x) + "_" + std::to_string(y) + ".png", 4);
		}
	}
	Texture::AddHorizontally("Charmadillo_padded_0_0.png", "Charmadillo_padded_1_0.png", "Charmadillo_sheet_0.png");
	Texture::AddHorizontally("Charmadillo_sheet_0.png", "Charmadillo_padded_2_0.png", "Charmadillo_sheet_0.png");
	Texture::AddHorizontally("Charmadillo_sheet_0.png", "Charmadillo_padded_2_0.png", "Charmadillo_sheet_0.png");

	Texture::AddHorizontally("Charmadillo_padded_0_1.png", "Charmadillo_padded_1_1.png", "Charmadillo_sheet_1.png");
	Texture::AddHorizontally("Charmadillo_sheet_1.png", "Charmadillo_padded_2_1.png", "Charmadillo_sheet_1.png");
	Texture::AddHorizontally("Charmadillo_sheet_1.png", "Charmadillo_padded_2_1.png", "Charmadillo_sheet_1.png");
	Texture::AddVertically("Charmadillo_sheet_0.png", "Charmadillo_sheet_1.png", "res/tmx/graphics/Charmadillo_sheet.png");

	for (unsigned int x = 0; x < 4; x++) {
		for (unsigned int y = 0; y < 2; y++) {
			Texture::CutSubimage("res/tmx/graphics/monsters/Finiette.png", "Finiette.png", x * 192u, y * 192u, 192u, 192u);
			Texture::AddRemoveBottomPadding("Finiette.png", "Finiette_padded.png", 4);
			Texture::AddRemoveTopPadding("Finiette_padded.png", "Finiette_padded.png", 4);
			Texture::AddRemoveLeftPadding("Finiette_padded.png", "Finiette_padded.png", 4);
			Texture::AddRemoveRightPadding("Finiette_padded.png", "Finiette_padded_" + std::to_string(x) + "_" + std::to_string(y) + ".png", 4);
		}
	}
	Texture::AddHorizontally("Finiette_padded_0_0.png", "Finiette_padded_1_0.png", "Finiette_sheet_0.png");
	Texture::AddHorizontally("Finiette_sheet_0.png", "Finiette_padded_2_0.png", "Finiette_sheet_0.png");
	Texture::AddHorizontally("Finiette_sheet_0.png", "Finiette_padded_2_0.png", "Finiette_sheet_0.png");

	Texture::AddHorizontally("Finiette_padded_0_1.png", "Finiette_padded_1_1.png", "Finiette_sheet_1.png");
	Texture::AddHorizontally("Finiette_sheet_1.png", "Finiette_padded_2_1.png", "Finiette_sheet_1.png");
	Texture::AddHorizontally("Finiette_sheet_1.png", "Finiette_padded_2_1.png", "Finiette_sheet_1.png");
	Texture::AddVertically("Finiette_sheet_0.png", "Finiette_sheet_1.png", "res/tmx/graphics/Finiette_sheet.png");

	for (unsigned int x = 0; x < 4; x++) {
		for (unsigned int y = 0; y < 2; y++) {
			Texture::CutSubimage("res/tmx/graphics/monsters/Pluma.png", "Pluma.png", x * 192u, y * 192u, 192u, 192u);
			Texture::AddRemoveBottomPadding("Pluma.png", "Pluma_padded.png", 4);
			Texture::AddRemoveTopPadding("Pluma_padded.png", "Pluma_padded.png", 4);
			Texture::AddRemoveLeftPadding("Pluma_padded.png", "Pluma_padded.png", 4);
			Texture::AddRemoveRightPadding("Pluma_padded.png", "Pluma_padded_" + std::to_string(x) + "_" + std::to_string(y) + ".png", 4);
		}
	}

	Texture::AddHorizontally("Pluma_padded_0_0.png", "Pluma_padded_1_0.png", "Pluma_sheet_0.png");
	Texture::AddHorizontally("Pluma_sheet_0.png", "Pluma_padded_2_0.png", "Pluma_sheet_0.png");
	Texture::AddHorizontally("Pluma_sheet_0.png", "Pluma_padded_2_0.png", "Pluma_sheet_0.png");

	Texture::AddHorizontally("Pluma_padded_0_1.png", "Pluma_padded_1_1.png", "Pluma_sheet_1.png");
	Texture::AddHorizontally("Pluma_sheet_1.png", "Pluma_padded_2_1.png", "Pluma_sheet_1.png");
	Texture::AddHorizontally("Pluma_sheet_1.png", "Pluma_padded_2_1.png", "Pluma_sheet_1.png");
	Texture::AddVertically("Pluma_sheet_0.png", "Pluma_sheet_1.png", "res/tmx/graphics/Pluma_sheet.png");


	Texture::AddHighlight("res/tmx/graphics/monsters/Atrox.png", "res/tmx/graphics/monsters/Atrox_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Charmadillo_sheet.png", "res/tmx/graphics/monsters/Charmadillo_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Cindrill.png", "res/tmx/graphics/monsters/Cindrill_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Cleaf.png", "res/tmx/graphics/monsters/Cleaf_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Draem.png", "res/tmx/graphics/monsters/Draem_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Finiette_sheet.png", "res/tmx/graphics/monsters/Finiette_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Finsta.png", "res/tmx/graphics/monsters/Finsta_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Friolera.png", "res/tmx/graphics/monsters/Friolera_highlight.png", 4);

	Texture::AddHighlight("res/tmx/graphics/monsters/Gulfin.png", "res/tmx/graphics/monsters/Gulfin_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Ivieron.png", "res/tmx/graphics/monsters/Ivieron_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Jacana.png", "res/tmx/graphics/monsters/Jacana_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Larvea.png", "res/tmx/graphics/monsters/Larvea_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Pluma_sheet.png", "res/tmx/graphics/monsters/Pluma_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Plumette.png", "res/tmx/graphics/monsters/Plumette_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Pouch.png", "res/tmx/graphics/monsters/Pouch_highlight.png", 4);
	Texture::AddHighlight("res/tmx/graphics/monsters/Sparchu.png", "res/tmx/graphics/monsters/Sparchu_highlight.png", 4);

	Texture::GrayScale("res/tmx/graphics/ui/sword.png", "res/tmx/graphics/ui/sword_gray.png");
	Texture::GrayScale("res/tmx/graphics/ui/shield.png", "res/tmx/graphics/ui/shield_gray.png");
	Texture::GrayScale("res/tmx/graphics/ui/arrows.png", "res/tmx/graphics/ui/arrows_gray.png");
	Texture::GrayScale("res/tmx/graphics/ui/hand.png", "res/tmx/graphics/ui/hand_gray.png");

	Texture::GrayScale("res/tmx/graphics/ui/sword_highlight.png", "res/tmx/graphics/ui/sword_gray_highlight.png");
	Texture::GrayScale("res/tmx/graphics/ui/shield_highlight.png", "res/tmx/graphics/ui/shield_gray_highlight.png");
	Texture::GrayScale("res/tmx/graphics/ui/arrows_highlight.png", "res/tmx/graphics/ui/arrows_gray_highlight.png");
	Texture::GrayScale("res/tmx/graphics/ui/hand_highlight.png", "res/tmx/graphics/ui/hand_gray_highlight.png");

	TextureAtlasCreator::Get().init(1536u, 768u);
	for (unsigned int x = 0; x < 24; x++) {
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 0u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 3u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 6u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 9u, 64u, 64u, false, false);

		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 1u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 4u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 7u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 10u, 64u, 64u, false, false);

		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 2u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 5u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 8u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 11u, 64u, 64u, false, false);
	}
	TileSetManager::Get().getTileSet("coast").loadTileSetGpu();
	Spritesheet::Safe("res/tmx/graphics/tilesets/coast_ordered", TileSetManager::Get().getTileSet("coast").getAtlas());

	//Bars post processing
	Texture::CreateBars("res/tmx/graphics/other/bars.png", 1024u, 256u, 200u, 5u);*/
}