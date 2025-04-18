#include <iostream>
#include "Application.h"

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt), m_eventDispatcher(new EventDispatcher()){
	initWindow();
	initOpenGL();
	loadAssets();
	initStates();

	m_enableVerticalSync = true;

	m_eventDispatcher->setProcessOSEvents([&]() {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	});
}

Application::~Application() {
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
		"Voyager",
		WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX),
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
					m_machine->toggleWireframe();
					break;
				}case VK_SPACE: {
					//  Mouse::instance().detach2();
					Keyboard::instance().disable();
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
			Globals::projection = Matrix4f::GetPerspective(Globals::projection, 45.0f, static_cast<float>(_width) / static_cast<float>(_height), 1.0f, 5000.0f);
			Globals::invProjection = Matrix4f::GetInvPerspective(Globals::invProjection, 45.0f, static_cast<float>(_width) / static_cast<float>(_height), 1.0f, 5000.0f);
			Globals::orthographic = Matrix4f::GetOrthographic(Globals::orthographic, -static_cast<float>(_width) * 0.5f, static_cast<float>(_width)* 0.5f, 0.0f, static_cast<float>(_height), -1.0f, 1.0f);
	
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
	//glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_LESS);
	//glDepthFunc(GL_ALWAYS);
	//alpha test for cutting border of the quads
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.0);

	glAlphaFunc(GL_GEQUAL, 0.5);

	

	//glDisable(GL_ALPHA_TEST);
	//button transparency, fog and light
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//https://stackoverflow.com/questions/2171085/opengl-blending-with-previous-contents-of-framebuffer
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//outline
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glEnable(GL_CLIP_DISTANCE0);
	
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
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
			event.mouseMove.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.mouseMove.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			m_eventDispatcher->pushEvent(event);
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

void Application::AddMouseListener(MouseEventListener * el) {
	m_eventDispatcher->AddMouseListener(el);
}

void Application::loadAssets() {

	Globals::shaderManager.loadShader("quad", "res/shader/quad.vs", "res/shader/quad.fs");
	Globals::shaderManager.loadShader("quad_shadow", "res/shader/quad_shadow.vs", "res/shader/quad_shadow.fs");
	Globals::shaderManager.loadShader("quad_array", "res/shader/quad_array.vs", "res/shader/quad_array.fs");
	Globals::shaderManager.loadShader("quad_array_shadow", "res/shader/quad_array_shadow.vs", "res/shader/quad_array_shadow.fs");
	Globals::shaderManager.loadShader("texture", "res/shader/texture.vs", "res/shader/texture.fs");
	Globals::shaderManager.loadShader("transperancy", "res/shader/transperancy.vs", "res/shader/transperancy.fs");
	Globals::shaderManager.loadShader("font", "res/shader/font.vs", "res/shader/font.fs");
	Globals::shaderManager.loadShader("fontDF", "res/shader/fontDF.vs", "res/shader/fontDF.fs");
	Globals::shaderManager.loadShader("skybox", "res/shader/skybox.vs", "res/shader/skybox.fs");
	Globals::shaderManager.loadShader("normal", "res/shader/normal.vs", "res/shader/normal.fs");
	Globals::shaderManager.loadShader("normal_map", "res/shader/normal_map.vs", "res/shader/normal_map.fs");
	Globals::shaderManager.loadShader("normalGS", "res/shader/normalGS.vs", "res/shader/normalGS.fs", "res/shader/normalGS.gs");
	Globals::shaderManager.loadShader("ray", "res/shader/ray.vs", "res/shader/ray.fs");
	Globals::shaderManager.loadShader("aabb", "res/shader/aabb.vs", "res/shader/aabb.fs");
	Globals::shaderManager.loadShader("color", "res/shader/color.vs", "res/shader/color.fs");

	Globals::shaderManager.loadShader("terrain", "res/shader/terrain.vs", "res/shader/terrain.fs");	
	Globals::shaderManager.loadShader("terrain_instance", "res/shader/terrain_instance.vs", "res/shader/terrain_instance.fs");
	Globals::shaderManager.loadShader("water", "res/shader/water.vs", "res/shader/water.fs");
	Globals::shaderManager.loadShader("depth", "res/shader/depth.vs", "res/shader/depth.fs");
	Globals::shaderManager.loadShader("depthGS", "res/shader/depthGS.vs", "res/shader/depthGS.fs", "res/shader/depthGS.gs");	
	Globals::shaderManager.loadShader("shadowQuad", "res/shader/shadowQuad.vs", "res/shader/shadowQuad.fs");
	Globals::shaderManager.loadShader("dissolve", "res/shader/dissolve.vs", "res/shader/dissolve.fs");
	Globals::shaderManager.loadShader("ring", "res/shader/ring.vs", "res/shader/ring.fs");

	Globals::textureManager.loadTexture("dirt", "res/textures/dirt.JPG", true);
	Globals::textureManager.loadTexture("grass", "res/textures/grass.JPG", true);
	Globals::textureManager.loadTexture("rock", "res/textures/rock.JPG", true);
	Globals::textureManager.loadTexture("snow", "res/textures/snow.JPG", true);
	Globals::textureManager.loadTexture("height_map", "res/heightmap.png", false);
	Globals::textureManager.createNullTexture("null");
	Globals::textureManager.createNullTexture("grey", 2, 2, 128);
	Globals::textureManager.loadTexture("marble", "res/textures/marble.png", true);

	Globals::textureManager.get("marble").setLinear();
	Globals::textureManager.get("marble").setRepeat();

	Globals::textureManager.loadTexture("tree", "res/models/tree/lowPolyTree.png", true);
	Globals::textureManager.get("tree").setLinear();

	Globals::textureManager.loadTexture("fern", "res/models/fern/fern.png", true);
	Globals::textureManager.get("fern").setLinear();

	Globals::textureManager.loadTexture("barrel", "res/models/barrel/barrel.png", true);
	Globals::textureManager.get("barrel").setLinear();
	
	Globals::textureManager.loadTexture("barrel_normal", "res/models/barrel/barrelNormal.png", true);
	Globals::textureManager.get("barrel_normal").setLinear();
	
	Globals::textureManager.loadTexture("woman", "Assets/Woman.png", false);

	Globals::spritesheetManager.createSpritesheet("terrain", Globals::textureManager.get("dirt").getTexture(), 512, 512, GL_RGB8);
	Globals::spritesheetManager.getAssetPointer("terrain")->addToSpritesheet((&Globals::textureManager.get("grass"))->getTexture(), 512, 512, GL_RGB8);
	Globals::spritesheetManager.getAssetPointer("terrain")->addToSpritesheet((&Globals::textureManager.get("rock"))->getTexture(), 512, 512, GL_RGB8);
	Globals::spritesheetManager.getAssetPointer("terrain")->addToSpritesheet((&Globals::textureManager.get("snow"))->getTexture(), 512, 512, GL_RGB8);	
	Globals::spritesheetManager.getAssetPointer("terrain")->setLinear();
	Globals::spritesheetManager.getAssetPointer("terrain")->setRepeat();

	Globals::spritesheetManager.createNullSpritesheet("null");
	Globals::spritesheetManager.getAssetPointer("null")->setLinear();
	Globals::spritesheetManager.getAssetPointer("null")->setRepeat();


	Globals::textureManager.loadTexture("grass2", "res/blend/grass.png", true);
	Globals::textureManager.loadTexture("mud", "res/blend/mud.png", true);
	Globals::textureManager.loadTexture("grass_flower", "res/blend/grassFlowers.png", true);
	Globals::textureManager.loadTexture("path", "res/blend/path.png", true);
	Globals::textureManager.loadTexture("blend_map", "res/blend/blendMap.png", true);
	Globals::textureManager.get("path").setLinear();
	Globals::textureManager.get("path").setRepeat();
	Globals::textureManager.get("blend_map").setLinear();
	Globals::textureManager.get("blend_map").setRepeat();

	
	Globals::spritesheetManager.createSpritesheet("blend", Globals::textureManager.get("grass2").getTexture(), 256, 256, GL_RGBA8);
	Globals::spritesheetManager.getAssetPointer("blend")->addToSpritesheet((&Globals::textureManager.get("mud"))->getTexture(), 256, 256, GL_RGBA8);
	Globals::spritesheetManager.getAssetPointer("blend")->addToSpritesheet((&Globals::textureManager.get("grass_flower"))->getTexture(), 256, 256, GL_RGBA8);
	Globals::spritesheetManager.getAssetPointer("blend")->setLinear();
	Globals::spritesheetManager.getAssetPointer("blend")->setRepeat();


	Globals::textureManager.loadTexture("water_dudv", "res/water/dudv.png", true);
	Globals::textureManager.loadTexture("water_normal", "res/water/normal.png", true);

	Globals::textureManager.get("water_dudv").setLinear();
	Globals::textureManager.get("water_dudv").setRepeat();
	Globals::textureManager.get("water_normal").setLinear();
	Globals::textureManager.get("water_normal").setRepeat();

	Globals::textureManager.createPerlinTexture("perlin", 512, 512, 373);
	Globals::textureManager.get("perlin").setLinear();
	Globals::textureManager.get("perlin").setRepeat();

	//Globals::textureManager.loadTexture("gradient", "res/gradient.png", true);
	//Globals::textureManager.get("gradient").setLinear();
	//Globals::textureManager.get("gradient").setRepeat();

	std::string facesDay[] = { "res/cubemap/day/right.png", "res/cubemap/day/left.png", "res/cubemap/day/top.png", "res/cubemap/day/bottom.png", "res/cubemap/day/back.png", "res/cubemap/day/front.png", };
	Globals::cubemapManager.loadCubeMap("day", facesDay, false);

	std::string facesNight[] = { "res/cubemap/night/right.png", "res/cubemap/night/left.png", "res/cubemap/night/top.png", "res/cubemap/night/bottom.png", "res/cubemap/night/back.png", "res/cubemap/night/front.png", };
	Globals::cubemapManager.loadCubeMap("night", facesNight, false);

	Globals::cubemapManager.createNullCubemap("null");
	
}