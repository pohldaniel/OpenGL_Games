#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION
#include <windows.h>			
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <tchar.h>
#include "stb\stb_image.h"

#include "GL.h"
#include "Extension.h"
#include "Camera.h"
#include "Quad.h"
#include "Character.h"
#include "Spritesheet.h"
#include "Spritesheet_old.h"

#define WIDTH 800
#define HEIGHT 600

#define MAP_TILE_DIM_X 50
#define MAP_TILE_DIM_Y 50
#define LEVEL_WIDTH 100
#define LEVEL_HEIGHT 12

const float xScale = MAP_TILE_DIM_X / (float)(WIDTH);
const float xTrans = xScale * 2.0f;
const float yScale = MAP_TILE_DIM_Y / (float)(HEIGHT);
const float yTrans = yScale * 2.0f;

POINT g_OldCursorPos;
bool g_enableVerticalSync;
bool g_enableWireframe;

enum DIRECTION {
	DIR_FORWARD = 1,
	DIR_BACKWARD = 2,
	DIR_LEFT = 4,
	DIR_RIGHT = 8,
	DIR_UP = 16,
	DIR_DOWN = 32,

	DIR_FORCE_32BIT = 0x7FFFFFFF
};

Camera camera;
Quad *quad, *quad2;
Character* character;
Spritesheet* level;
SpritesheetOld* level2;
//prototype funktions
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void enableWireframe(bool enableWireframe);
void enableVerticalSync(bool enableVerticalSync);

void initApp(HWND hWnd);
void processInput(HWND hWnd);
void loadLevel();
int levelMatrix[LEVEL_HEIGHT][LEVEL_WIDTH];

std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
std::chrono::steady_clock::time_point begin = end;
float elapsed_secs = 0.0f;
int frames = 0;
double framesTime = 0;

// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d, mouse : move camera" << std::endl;
	std::cout << "arrow keys        : move sphere" << std::endl;
	std::cout << "space             : release capture" << std::endl;
	std::cout << "v                 : toggle vsync" << std::endl;
	std::cout << "z                 : toggle wireframe" << std::endl;

	WNDCLASSEX		windowClass;		// window class
	HWND			hwnd;				// window handle
	MSG				msg;				// message
	HDC				hdc;				// device context handle

										// fill out the window class structure
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = winProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// default icon
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);			// default arrow
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// white background
	windowClass.lpszMenuName = NULL;									// no menu
	windowClass.lpszClassName = "WINDOWCLASS";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);			// windows logo small icon

																// register the windows class
	if (!RegisterClassEx(&windowClass))
		return 0;

	// class registered, so now create our window
	hwnd = CreateWindowEx(
		NULL,									// extended style
		"WINDOWCLASS",						// class name
		"Platfromer",					// app name
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,									// x,y coordinate
		WIDTH,
		HEIGHT,									// width, height
		NULL,									// handle to parent
		NULL,									// handle to menu
		hInstance,							// application instance
		NULL);								// no extra params

											// check if window creation failed (hwnd would equal NULL)
	if (!hwnd)
		return 0;

	// Adjust it so the client area is RESOLUTION_X/RESOLUTION_Y
	RECT rect1;
	GetWindowRect(hwnd, &rect1);
	RECT rect2;
	GetClientRect(hwnd, &rect2);

	SetWindowPos(
		hwnd,
		NULL,
		rect1.left,
		rect1.top,
		WIDTH + ((rect1.right - rect1.left) - (rect2.right - rect2.left)),
		HEIGHT + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)),
		NULL
	);

	ShowWindow(hwnd, SW_SHOW);			// display the window
	UpdateWindow(hwnd);					// update the window

	initApp(hwnd);


	// main message loop
	while (true) {

		end = std::chrono::high_resolution_clock::now();
		elapsed_secs = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
		begin = end;

		// Display FPS
		/*framesTime += elapsed_secs;
		frames++;
		if (framesTime > 1) {
		_TCHAR fpsText[32];
		_sntprintf(fpsText, 32, "Game: %d FPS", frames);
		SetWindowText(hwnd, fpsText);
		frames = 0;
		framesTime = 0;
		}*/


		// Did we recieve a message, or are we idling ?
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// test if this is a quit
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else {

			glClearColor(0.0f, 0.60f, 0.86f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(quad->getShader()->m_program);
			for (int y = 0; y < LEVEL_HEIGHT; y++) {
				for (int x = 0; x < LEVEL_WIDTH; x++) {
					if (levelMatrix[y][x] != -1) {
						quad->getShader()->loadMatrix("transform", Matrix4f::Translate((x + 0.5f) * xTrans - 1.0f, 1.0f - yTrans * (y + 0.5f), 0.0f));
						quad->getShader()->loadInt("layer", levelMatrix[y][x]);
						quad->render2(level->getAtlas());
					}

				}
			}
			glUseProgram(0);

			/*glUseProgram(quad2->getShader()->m_program);
			for (int y = 0; y < LEVEL_HEIGHT; y++){
				for (int x = 0; x < LEVEL_WIDTH; x++){
					if (levelMatrix[y][x] != -1) {
						quad2->getShader()->loadMatrix("transform", Matrix4f::Scale(xScale, yScale, 0.0f) * Matrix4f::Translate((x +  0.5f) * xTrans - 1.0f, 1.0f - yTrans * (y + 0.5f), 0.0f));
						quad2->getShader()->loadMatrix("frame", level2->getFrameTransform(levelMatrix[y][x]));
						quad2->render2(level2->getTexture());
					}
				}
			}
			glUseProgram(0);*/

			character->render2();

			processInput(hwnd);
			hdc = GetDC(hwnd);
			SwapBuffers(hdc);
			ReleaseDC(hwnd, hdc);
		}
	} // end while
	return msg.wParam;
}

// the Windows Procedure event handler
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context
	POINT pt;
	RECT rect;

	switch (message) {

	case WM_DESTROY: {

		PostQuitMessage(0);
		return 0;
	}

	case WM_CREATE: {

		GetClientRect(hWnd, &rect);
		g_OldCursorPos.x = rect.right / 2;
		g_OldCursorPos.y = rect.bottom / 2;
		pt.x = rect.right / 2;
		pt.y = rect.bottom / 2;
		//SetCursorPos(pt.x, pt.y);
		// set the cursor to the middle of the window and capture the window via "SendMessage"
		SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
		return 0;
	}break;

	case WM_LBUTTONDOWN: { // Capture the mouse

						   //setCursortoMiddle(hWnd);
						   //SetCapture(hWnd);

		return 0;
	} break;

	case WM_KEYDOWN: {

		switch (wParam) {

		case VK_ESCAPE: {

			PostQuitMessage(0);
			return 0;

		}break;
		case VK_SPACE: {

			ReleaseCapture();
			return 0;

		}break;

		case 'v': case 'V': {
			enableVerticalSync(!g_enableVerticalSync);
			return 0;

		}break;
		case 'z': case 'Z': {
			enableWireframe(!g_enableWireframe);
		}break;

			return 0;
		}break;

		return 0;
	}break;

	case WM_SIZE: {

		int _height = HIWORD(lParam);		// retrieve width and height
		int _width = LOWORD(lParam);

		if (_height == 0) {					// avoid divide by zero
			_height = 1;
		}
		//std::cout << _width << std::endl;
		//std::cout << _height << std::endl;
		glViewport(0, 0, _width, _height);
		camera.orthographic(0.0f, static_cast<float>(_width), static_cast<float>(_height), 0.0f, -1.0f, 1.0f);

		return 0;
	}break;

	default:
		break;
	}
	return (DefWindowProc(hWnd, message, wParam, lParam));
}

void initApp(HWND hWnd) {

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context

	hDC = GetDC(hWnd);
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

	glEnable(GL_DEPTH_TEST);					// hidden surface removal
												//glEnable(GL_CULL_FACE);					// do not calculate inside of poly's

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//(TILE_SIZE / 2) * ( 2.0f / (float)(WIDTH))

	level = new Spritesheet("tileset.png", MAP_TILE_DIM_X, MAP_TILE_DIM_Y, true, true);
	level2 = new SpritesheetOld("tileset.png", MAP_TILE_DIM_X, MAP_TILE_DIM_Y, true, true);

	quad = new Quad("shader/quad_array.vs", "shader/quad_array.fs", xScale, yScale);
	quad2 = new Quad("shader/quad2.vs", "shader/quad2.fs");

	loadLevel();
	character = new Character();
}

void setCursortoMiddle(HWND hwnd) {
	RECT rect;

	GetClientRect(hwnd, &rect);
	SetCursorPos(rect.right / 2, rect.bottom / 2);
}

void enableVerticalSync(bool enableVerticalSync) {

	// WGL_EXT_swap_control.
	typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(GLint);

	static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(
			wglGetProcAddress("wglSwapIntervalEXT"));

	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(enableVerticalSync ? 1 : 0);
		g_enableVerticalSync = enableVerticalSync;
	}
}

void enableWireframe(bool enableWireframe) {

	g_enableWireframe = enableWireframe;

	if (g_enableWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void loadLevel() {

	// Default nothing map
	for (int j = 0; j < LEVEL_HEIGHT; j++){
		for (int i = 0; i < LEVEL_WIDTH; i++){
			levelMatrix[j][i] = -1;
		}
	}

	// Load map from file
	std::ifstream myfile("level.txt");

	if (myfile.is_open()) {
		for (int j = 0; j < LEVEL_HEIGHT; j++) {
			for (int i = 0; i < LEVEL_WIDTH; i++) {
				int tmp;
				myfile >> tmp;
				levelMatrix[j][i] = tmp;
			}
		}
	}
}

void processInput(HWND hWnd) {

	static UCHAR pKeyBuffer[256];
	ULONG        Direction = 0;
	POINT        CursorPos;
	float        X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if (!GetKeyboardState(pKeyBuffer)) return;

	// Check the relevant keys
	if (pKeyBuffer[VK_UP] & 0xF0) Direction |= DIR_FORWARD;
	if (pKeyBuffer[VK_DOWN] & 0xF0) Direction |= DIR_BACKWARD;
	if (pKeyBuffer[VK_LEFT] & 0xF0) Direction |= DIR_LEFT;
	if (pKeyBuffer[VK_RIGHT] & 0xF0) Direction |= DIR_RIGHT;
	//if (Direction & DIR_RIGHT) {
	character->GoesRight = Direction & DIR_RIGHT;
	character->update(elapsed_secs);
	//}		

	//if (Direction & DIR_LEFT) {
	character->GoesLeft = Direction & DIR_LEFT;
	//}
}