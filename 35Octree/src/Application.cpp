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

#include <States/OctreeInterface.h>
#include <States/Default.h>
#include <States/Menu.h>
#include <States/RayMarch.h>
#include <States/Particle/Fire.h>
#include <States/BlobShoot.h>

#include <States/Particle/BlendedParticle.h>
#include <States/Particle/SmoothParticle.h>
#include <States/Particle/SoftParticle.h>

#include <States/SSS/SSSGems.h>
#include <States/SSS/SSSApproximation.h>
#include <States/SSS/SSSOGLP.h>
#include <States/SSS/Separable.h>

#include <States/Shadow/Shadow.h>
#include <States/Stencil/Stencil.h>

#include <States/Shell/Shell1.h>
#include <States/Shell/Shell2.h>
#include <States/Shell/Shell3.h>

#include <States/Grass/GrassComp.h>
#include <States/Grass/GrassGeom.h>

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
	initOpenGL();
	showWindow();
	initImGUI();
	//initOpenAL();
	loadAssets();

	Framebuffer::SetDefaultSize(Width, Height);
	Widget::Init(Width, Height);
	Sprite::Init(Width, Height);

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

	Batchrenderer::Get().init(1000, false, true);
	Batchrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("batch"));

	DebugRenderer::Get().init();
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

	ImGui::DestroyContext();

	HDC hdc = GetDC(Window);
	wglMakeCurrent(hdc, 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(Window, hdc);

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
		"Octree",
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

	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

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
	//Machine->addStateAtTop(new Menu(*Machine));

	Machine->addStateAtTop(new OctreeInterface(*Machine));
	//Machine->addStateAtTop(new Default(*Machine));
	//Machine->addStateAtTop(new RayMarch(*Machine));
	//Machine->addStateAtTop(new Fire(*Machine));
	//Machine->addStateAtTop(new BlendedParticle(*Machine));
	//Machine->addStateAtTop(new SmoothParticle(*Machine));
	//Machine->addStateAtTop(new SoftParticle(*Machine));
	//Machine->addStateAtTop(new BlobShoot(*Machine));
	//Machine->addStateAtTop(new SSSApproximation(*Machine));
	//Machine->addStateAtTop(new SSSGems(*Machine));
	//Machine->addStateAtTop(new SSSOGLP(*Machine));	
	//Machine->addStateAtTop(new Separable(*Machine));
	//Machine->addStateAtTop(new Stencil(*Machine));
	//Machine->addStateAtTop(new Shadow(*Machine));
	//Machine->addStateAtTop(new Shell1(*Machine));
	//Machine->addStateAtTop(new Shell2(*Machine));
	//Machine->addStateAtTop(new Shell3(*Machine));
	//Machine->addStateAtTop(new GrassComp(*Machine));
	//Machine->addStateAtTop(new GrassGeom(*Machine));
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

	Globals::shaderManager.loadShader("texture", "res/shader/texture.vert", "res/shader/texture.frag");	
	Globals::shaderManager.loadShader("font", "res/shader/batch.vert", "res/shader/font.frag");
	Globals::shaderManager.loadShader("batch", "res/shader/batch.vert", "res/shader/batch.frag");
	Globals::shaderManager.loadShader("ray_march", "res/shader/ray_march.vert", "res/shader/ray_march.frag");
	Globals::shaderManager.loadShader("ray_march_new", "res/shader/ray_march_new.vert", "res/shader/ray_march_new.frag");
	Globals::shaderManager.loadShader("scene", "res/shader/scene.vert", "res/shader/scene.frag");
	Globals::shaderManager.loadShader("model", "res/shader/model.vert", "res/shader/model.frag");
	Globals::shaderManager.loadShader("fire", "res/shader/fire.vert", "res/shader/fire.frag", "res/shader/fire.gem");
	Globals::shaderManager.loadShader("skybox", "res/shader/skybox.vert", "res/shader/skybox.frag");
	Globals::shaderManager.loadShader("particle", "res/shader/particle.vert", "res/shader/particle.frag");
	Globals::shaderManager.loadShader("particle_blend", "res/shader/particle_blend.vert", "res/shader/particle_blend.frag");
	Globals::shaderManager.loadShader("particle_smooth", "res/shader/particle_smooth.vert", "res/shader/particle_smooth.frag");
	Globals::shaderManager.loadShader("particle_soft", "res/shader/particle_soft.vert", "res/shader/particle_soft.frag");
	Globals::shaderManager.loadShader("particle_normal", "res/shader/particle_normal.vert", "res/shader/particle_normal.frag");
	Globals::shaderManager.loadShader("color", "res/shader/color.vert", "res/shader/color.frag");
	Globals::shaderManager.loadShader("geometry", "res/shader/normalGS.vert", "res/shader/normalGS.frag", "res/shader/normalGS.gem");
	Globals::shaderManager.loadShader("depth", "res/shader/depth.vert", "res/shader/depth.frag");
	//Globals::shaderManager.loadShader("combiner", "res/shader/combiner.vert", "res/shader/combiner.frag");

	Shader::SetIncludeFromFile("SeparableSSS.h", "res/shader/PBR/SeparableSSS.h");
	Globals::shaderManager.loadShader("pbr", "res/shader/PBR/pbr.vert.glsl", "res/shader/PBR/pbr.frag.glsl");
	Globals::shaderManager.loadShader("approximate", "res/shader/SSS/approximate.vert", "res/shader/SSS/approximate.frag");
	Globals::shaderManager.loadShader("irradiance", "res/shader/SSS/irradiance.vert", "res/shader/SSS/irradiance.frag");
	Globals::shaderManager.loadShader("distance", "res/shader/SSS/distance.vert", "res/shader/SSS/distance.frag");

	Globals::shaderManager.loadShader("depth_sep", "res/shader/SSS/depth_sep.vert", "res/shader/SSS/depth_sep.frag");
	Globals::shaderManager.loadShader("main_sep", "res/shader/SSS/main_sep.vert", "res/shader/SSS/main_sep.frag");
	Globals::shaderManager.loadShader("sss_sep", "res/shader/SSS/sss_sep.vert", "res/shader/SSS/sss_sep.frag");
	Globals::shaderManager.loadShader("spec_sep", "res/shader/SSS/specular_sep.vert", "res/shader/SSS/specular_sep.frag");

	Globals::shaderManager.loadShader("depth_gems", "res/shader/SSS/depth_gems.vert", "res/shader/SSS/depth_gems.frag");
	Globals::shaderManager.loadShader("main_gems", "res/shader/SSS/main_gems.vert", "res/shader/SSS/main_gems.frag");	
	Globals::shaderManager.loadShader("depth_oglp", "res/shader/SSS/depth_oglp.vert", "res/shader/SSS/depth_oglp.frag");
	Globals::shaderManager.loadShader("main_oglp", "res/shader/SSS/main_oglp.vert", "res/shader/SSS/main_oglp.frag", "res/shader/SSS/main_oglp.gem");

	Globals::shaderManager.loadShader("stencil_1", "res/shader/Stencil/stencil_1.vert", "res/shader/Stencil/stencil_1.frag");

	Globals::shaderManager.loadShader("shadow_base", "res/shader/Shadow/base.vert", "res/shader/Shadow/base.frag");
	Globals::shaderManager.loadShader("shadow", "res/shader/Shadow/shadow.vert", "res/shader/Shadow/shadow.frag");

	Globals::shaderManager.loadShader("fur_base", "res/shader/Shell/base.vert", "res/shader/Shell/base.frag");
	Globals::shaderManager.loadShader("fur", "res/shader/Shell/fur.vert", "res/shader/Shell/fur.frag", "res/shader/Shell/fur.gem");
	Globals::shaderManager.loadShader("shell", "res/shader/Shell/shell.vert", "res/shader/Shell/shell.frag", "res/shader/Shell/shell.gem");
	Globals::shaderManager.loadShader("grass_basic", "res/shader/Shell/grass_basic.vert", "res/shader/Shell/grass_basic.frag");
	Globals::shaderManager.loadShader("grass", "res/shader/Shell/grass.vert", "res/shader/Shell/grass.frag");

	Globals::shaderManager.loadShader("grass_tess", "res/shader/Grass/grass.vert.glsl", "res/shader/Grass/grass.frag.glsl", "res/shader/Grass/grass.tesc.glsl", "res/shader/Grass/grass.tese.glsl");
	Globals::shaderManager.loadShader("grass_comp", "res/shader/Grass/grass.comp.glsl");
	Globals::shaderManager.loadShader("grass_gem", "res/shader/Grass/grass.vs.glsl", "res/shader/Grass/grass.fs.glsl", "res/shader/Grass/grass.gs.glsl");

	Globals::fontManager.loadCharacterSet("upheaval_200", "res/fonts/upheavtt.ttf", 200, 0, 30, 128, 0, true, 0u);
	Globals::fontManager.loadCharacterSet("upheaval_50", "res/fonts/upheavtt.ttf", 30, 0, 3, 0, 0, true, 0u);

	Globals::textureManager.loadTexture("forest_1", "res/backgrounds/Forest/plx-1.png");
	Globals::textureManager.loadTexture("forest_2", "res/backgrounds/Forest/plx-2.png");
	Globals::textureManager.loadTexture("forest_3", "res/backgrounds/Forest/plx-3.png");
	Globals::textureManager.loadTexture("forest_4", "res/backgrounds/Forest/plx-4.png");
	Globals::textureManager.loadTexture("forest_5", "res/backgrounds/Forest/plx-5.png");
	Globals::textureManager.createNullTexture("null");
	Globals::textureManager.loadTexture("marble", "res/textures/marble.png", true);

	Globals::textureManager.loadTexture("flame", "res/textures/Flame_Particle.png", false);
	Globals::textureManager.get("flame").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("flame").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("pine", "res/textures/Log_pine_color.png", true);
	Globals::textureManager.get("pine").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("pine").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("pine_normal", "res/textures/Log_pine_normal.png", true);
	Globals::textureManager.get("pine_normal").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("pine_normal").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("grass", "res/textures/Grass.png", true);
	Globals::textureManager.get("grass").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("grass").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("grass_normal", "res/textures/Grass_normal.png", true);
	Globals::textureManager.get("grass_normal").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("grass_normal").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("statue", "res/models/statue.dds", true);
	Globals::textureManager.get("statue").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("statue").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("grass_green", "res/textures/grass_green.jpg", true);
	Globals::textureManager.get("grass_green").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("grass_green").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("grass_bill", "res/textures/grass_texture.png", true);
	Globals::textureManager.get("grass_bill").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("grass_bill").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("flow_map", "res/textures/flowmap.png", true);
	Globals::textureManager.get("flow_map").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("flow_map").setWrapMode(GL_REPEAT);

	if (!Utils::fileExist("res/textures/fire_pre.png")) {
		Texture::PremultiplyAlpha("res/textures/fire.png", "res/textures/fire_pre.png");
	}
	Globals::textureManager.loadTexture("fire", "res/textures/fire_pre.png", true);
	Globals::textureManager.get("fire").setFilter(GL_LINEAR);
	Globals::textureManager.loadTexture("grid", "res/textures/grid512.png");

	Globals::textureManager.loadTexture("smoke", "res/textures/smoke.png", false);
	Globals::textureManager.get("smoke").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("smoke").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("bricks", "res/textures/bricks.png", true);
	Globals::textureManager.get("bricks").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("bricks").setWrapMode(GL_REPEAT);

	Globals::textureManager.loadTexture("bricks_disp", "res/textures/bricks_disp.png", true);
	Globals::textureManager.get("bricks_disp").setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Globals::textureManager.get("bricks_disp").setWrapMode(GL_REPEAT);
	Globals::textureManager.get("bricks_disp").generateDisplacements();

	std::string faces[] = { "res/textures/posx.jpg", "res/textures/negx.jpg", "res/textures/posy.jpg", "res/textures/negy.jpg", "res/textures/posz.jpg", "res/textures/negz.jpg" };
	Globals::textureManager.loadCubeMap("dessert", faces, false);

	std::string faces2[] = { "res/textures/ely_hills/hills_rt.tga", "res/textures/ely_hills/hills_lf.tga", "res/textures/ely_hills/hills_up.tga", "res/textures/ely_hills/hills_dn.tga", "res/textures/ely_hills/hills_bk.tga", "res/textures/ely_hills/hills_ft.tga" };
	Globals::textureManager.loadCubeMap("hills", faces2, false);

	Globals::shapeManager.buildSphere("sphere", 0.5f, Vector3f(0.0f, 0.0f, 0.0f), 16, 16, true, true, false);
	Globals::shapeManager.buildCube("cube", Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), 1, 1, true, true, false);
	Globals::shapeManager.get("cube").createBoundingBox();
	Globals::shapeManager.buildQuadXY("quad", Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), 1, 1, true, false, false);
	Globals::shapeManager.buildQuadXZ("quad_xz", Vector3f(-1.0f, 0.0f, -1.0f), Vector2f(2.0f, 2.0f), 1, 1, true, false, false);
	Globals::shapeManager.buildTorus("torus", 1.0f, 0.4f, Vector3f(0.0f, 0.0f, 0.0f), 49, 49, true, true, true);
	Globals::shapeManager.buildTorusKnot("torus_knot", 2.0f, 0.75f, 2, 3, Vector3f(0.0f, 0.0f, 0.0f), 100, 16, true, true, false);
	Globals::shapeManager.buildTorus("torus_shell", 1.0f, 0.5f, Vector3f(0.0f, 0.0f, 0.0f), 72, 48, true, true, true);
	Globals::shapeManager.buildQuadXZ("grass_blade", Vector3f(-1.0f, 0.0f, -1.0f), Vector2f(2.0f, 2.0f), 1, 1, true, true, true);
	Globals::shapeManager.buildQuadXZ("floor", Vector3f(-50.0f, 0.0f, -50.0f), Vector2f(100.0f, 100.0f), 10, 10, true, true, false);
	Globals::shapeManager.buildQuadXZ("floor_shadow", Vector3f(-500.0f, -50.0f, -500.0f), Vector2f(1000.0f, 1000.0f), 1, 1, false, true, false);
	Globals::shapeManager.buildQuadXZ("floor_grass", Vector3f(-20.0f, 0.0f, -20.0f), Vector2f(39.0f, 39.0f), 1, 1, true, false, false);
}