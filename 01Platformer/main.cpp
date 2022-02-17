#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION
#include <windows.h>			
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <tchar.h>
#include <list>
#include "stb\stb_image.h"

#include "GameObject.h"
#include "Extension.h"
#include "Camera.h"
#include "Level.h"
#include "Character.h"


#define WIDTH 800
#define HEIGHT 600

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

Character* character;
Level* level;

//prototype funktions
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void enableWireframe(bool enableWireframe);
void enableVerticalSync(bool enableVerticalSync);

void initApp(HWND hWnd);
void processInput(HWND hWnd);


std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
std::chrono::steady_clock::time_point begin = end;
float elapsed_secs = 0.0f;
int frames = 0;
double framesTime = 0;


void addGameObject(GameObject* gameObj);
void removeGameObject(GameObject* gameObj);
std::list<GameObject*> objectList;

boolean leftPressed;
boolean rightPressed;
boolean spacePressed;

// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
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
			processInput(hwnd);
			///////////////////////////////////////////////////////////////////////////
			std::list<GameObject*>::iterator iter;

			for (iter = objectList.begin(); iter != objectList.end(); iter++)
				(*iter)->update(elapsed_secs);

			
			for (iter = objectList.begin(); iter != objectList.end(); iter++)
				(*iter)->render();

			///////////////////////////////////////////////////////////////////////////
			character->goesLeft = leftPressed;
			character->goesRight = rightPressed;
			
			if (!character->isDead()) {
				CollisionDistances cd = level->characterCollides(character);
				if (cd.bottom > 0){
					// if it's a bottom collision, we stop the character from falling
					character->stopFalling(cd.bottom);
					if (spacePressed){ // if the user pressed the jump key, the character jumps
						character->jump(true);
						spacePressed = false;
					}
				}

				if (cd.left > 0){
					// if it's a left collision
					character->stopMovingLeft(cd.left);
				}

				if (cd.right > 0){
					// if it's a right collision
					character->stopMovingRight(cd.right);
				}

				if (cd.top > 0){
					// if it's a top collision
					character->bounceTop();
				}

				if (character->getPosition()[1] > HEIGHT){
					// If the character falls below the level, it dies
					character->die();
				}
				
				// Change the display offset based on character position, but clamp it to the limits
				offset = character->getPosition()[0] - WIDTH/ 2;
				if (offset < 0)
					offset = 0;
				if (offset > LEVEL_WIDTH* TILE_WIDTH - WIDTH)
					offset = LEVEL_WIDTH * TILE_WIDTH - WIDTH;
			}
			///////////////////////////////////////////////////////////////////////////

			
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
		glViewport(0, 0, _width, _height);
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

	glEnable(GL_DEPTH_TEST);					
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	character = new Character();
	level = new Level();

	level = new Level();
	addGameObject(level);

	// Initialize character
	character = new Character();
	addGameObject(character);
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
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	if (pKeyBuffer[VK_LEFT] & 0xF0) Direction |= DIR_LEFT;
	if (pKeyBuffer[VK_RIGHT] & 0xF0) Direction |= DIR_RIGHT;
	if (pKeyBuffer[VK_UP] & 0xF0) Direction |= DIR_UP;
	if (pKeyBuffer[VK_SPACE] & 0xF0) Direction |= DIR_UP;

	rightPressed = Direction & DIR_RIGHT;
	leftPressed = Direction & DIR_LEFT;
	spacePressed = Direction & DIR_UP;
}

void addGameObject(GameObject* gameObj) {
	objectList.push_back(gameObj);
}

void removeGameObject(GameObject* gameObj) {
	objectList.remove(gameObj);
}