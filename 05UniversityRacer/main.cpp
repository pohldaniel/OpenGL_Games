#define NOMINMAX

#include <windows.h>			
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>


#include "GL.h"
#include "Extension.h"
#include "Vector.h"

#include "MeshObject\MeshQuad.h"
#include "ObjModel.h"
#include "Camera.h"
#include "Camera2.h"
#include "Physics.h"

#define UNUSED_SHADER_ATTR -1

#define WALK_SPEED 0.01f
#define STATICS_SCALE 0.05f
#define INTRO_TIME_MS 2000

int height = 960;
int width = 1280;

POINT g_OldCursorPos;
bool g_enableVerticalSync;
bool g_enableWireframe;
bool followCamera = true;
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
Camera2 camera2;
MeshQuad *quad;
Model *room, *car, *wheel[4];
Physics *physics;

bool UPDATEFRAME = true;
double delta_Time;
//prototype funktions
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void enableWireframe(bool enableWireframe);
void enableVerticalSync(bool enableVerticalSync);

void initApp(HWND hWnd);
void processInput(HWND hWnd);
void updateFrame(HWND hWnd, double elapsedTimeSec);
// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d, q, e, mouse : move camera" << std::endl;
	std::cout << "arrow keys              : move player" << std::endl;
	std::cout << "control right           : jump" << std::endl;
	std::cout << "mouse left              : shoot sphere" << std::endl;
	std::cout << "space                   : release capture" << std::endl;
	std::cout << "v                       : toggle vsync" << std::endl;
	std::cout << "z                       : toggle wireframe" << std::endl;

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
	hwnd = CreateWindowEx(NULL,									// extended style
		"WINDOWCLASS",						// class name
		"Physics",					// app name
		WS_OVERLAPPEDWINDOW,
		0, 0,									// x,y coordinate
		width,
		height,									// width, height
		NULL,									// handle to parent
		NULL,									// handle to menu
		hInstance,							// application instance
		NULL);								// no extra params

											// check if window creation failed (hwnd would equal NULL)
	if (!hwnd)
		return 0;

	ShowWindow(hwnd, SW_SHOW);			// display the window
	UpdateWindow(hwnd);					// update the window

	initApp(hwnd);

	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point end;
	std::chrono::duration<double> deltaTime;

	// main message loop
	while (true) {

		// Did we recieve a message, or are we idling ?
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			// test if this is a quit
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}else {

			end = start;
			start = std::chrono::high_resolution_clock::now();
			deltaTime = start - end;
			delta_Time = deltaTime.count();

			// fyzika -------------------------------------------------
			//physics->StepSimulation(deltaTime.count());
			physics->StepSimulation(1.0 / 60.0);

			glm::mat4 carMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetWorldTransform()), glm::vec3(CAR_SCALE));

			if (followCamera)
			{
				btVector3 vel = physics->GetCar()->GetVehicle()->getRigidBody()->getLinearVelocity();
				camera2.Follow(carMatrix, glm::vec3(vel.x(), vel.y(), vel.z()), deltaTime.count());
			}

			for (int i = 0; i < physics->GetCar()->GetVehicle()->getNumWheels(); i++)
			{
				physics->GetCar()->GetVehicle()->updateWheelTransform(i, true); //synchronize the wheels with the (interpolated) chassis worldtransform        
				glm::mat4 wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetVehicle()->getWheelInfo(i).m_worldTransform), glm::vec3(CAR_SCALE));

				if (i == PhysicsCar::WHEEL_FRONTRIGHT || i == PhysicsCar::WHEEL_REARRIGHT)
					wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));


			}
			// ---------------------------------------------------------
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(1.0, 1.0, 1.0, 1.0);
			// pohledova a projekcni matice - pro kresleni stinovych teles
			glm::mat4 mView = camera2.GetMatrix();
			glm::mat4 mPerspective = glm::perspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);

			camera.m_viewMatrix[0][0] = mView[0][0]; camera.m_viewMatrix[0][1] = mView[0][1]; camera.m_viewMatrix[0][2] = mView[0][2]; camera.m_viewMatrix[0][3] = mView[0][3];
			camera.m_viewMatrix[1][0] = mView[1][0]; camera.m_viewMatrix[1][1] = mView[1][1]; camera.m_viewMatrix[1][2] = mView[1][2]; camera.m_viewMatrix[1][3] = mView[1][3];
			camera.m_viewMatrix[2][0] = mView[2][0]; camera.m_viewMatrix[2][1] = mView[2][1]; camera.m_viewMatrix[2][2] = mView[2][2]; camera.m_viewMatrix[2][3] = mView[2][3];
			camera.m_viewMatrix[3][0] = mView[3][0]; camera.m_viewMatrix[3][1] = mView[3][1]; camera.m_viewMatrix[3][2] = mView[3][2]; camera.m_viewMatrix[3][3] = mView[3][3];

			camera.m_projMatrix[0][0] = mPerspective[0][0]; camera.m_projMatrix[0][1] = mPerspective[0][1]; camera.m_projMatrix[0][2] = mPerspective[0][2]; camera.m_projMatrix[0][3] = mPerspective[0][3];
			camera.m_projMatrix[1][0] = mPerspective[1][0]; camera.m_projMatrix[1][1] = mPerspective[1][1]; camera.m_projMatrix[1][2] = mPerspective[1][2]; camera.m_projMatrix[1][3] = mPerspective[1][3];
			camera.m_projMatrix[2][0] = mPerspective[2][0]; camera.m_projMatrix[2][1] = mPerspective[2][1]; camera.m_projMatrix[2][2] = mPerspective[2][2]; camera.m_projMatrix[2][3] = mPerspective[2][3];
			camera.m_projMatrix[3][0] = mPerspective[3][0]; camera.m_projMatrix[3][1] = mPerspective[3][1]; camera.m_projMatrix[3][2] = mPerspective[3][2]; camera.m_projMatrix[3][3] = mPerspective[3][3];


			car->setModelMatrix(Matrix4f(carMatrix[0][0], carMatrix[0][1], carMatrix[0][2], carMatrix[0][3],
				carMatrix[1][0], carMatrix[1][1], carMatrix[1][2], carMatrix[1][3],
				carMatrix[2][0], carMatrix[2][1], carMatrix[2][2], carMatrix[2][3],
				carMatrix[3][0], carMatrix[3][1], carMatrix[3][2], carMatrix[3][3]));


			physics->GetCar()->GetVehicle()->updateWheelTransform(0, true); //synchronize the wheels with the (interpolated) chassis worldtransform
			glm::mat4 wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetVehicle()->getWheelInfo(0).m_worldTransform), glm::vec3(CAR_SCALE));

			if (0 == PhysicsCar::WHEEL_FRONTRIGHT || 0 == PhysicsCar::WHEEL_REARRIGHT)
				wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

			wheel[0]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
				wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
				wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
				wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

			physics->GetCar()->GetVehicle()->updateWheelTransform(1, true);
			wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetVehicle()->getWheelInfo(1).m_worldTransform), glm::vec3(CAR_SCALE));

			if (1 == PhysicsCar::WHEEL_FRONTRIGHT || 1 == PhysicsCar::WHEEL_REARRIGHT)
				wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

			wheel[1]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
				wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
				wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
				wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

			physics->GetCar()->GetVehicle()->updateWheelTransform(2, true);
			wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetVehicle()->getWheelInfo(2).m_worldTransform), glm::vec3(CAR_SCALE));

			if (2 == PhysicsCar::WHEEL_FRONTRIGHT || 2 == PhysicsCar::WHEEL_REARRIGHT)
				wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

			wheel[2]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
				wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
				wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
				wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

			physics->GetCar()->GetVehicle()->updateWheelTransform(3, true);
			wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetVehicle()->getWheelInfo(3).m_worldTransform), glm::vec3(CAR_SCALE));

			if (3 == PhysicsCar::WHEEL_FRONTRIGHT || 3 == PhysicsCar::WHEEL_REARRIGHT)
				wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

			wheel[3]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
				wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
				wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
				wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

			car->draw(camera);
			room->draw(camera);
			wheel[0]->draw(camera);
			wheel[1]->draw(camera);
			wheel[2]->draw(camera);
			wheel[3]->draw(camera);

			glDisable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			processInput(hwnd);
			updateFrame(hwnd, delta_Time);
			hdc = GetDC(hwnd);
			SwapBuffers(hdc);
			ReleaseDC(hwnd, hdc);
		}
	} // end while

	delete quad;
	delete room;
	delete car;
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
		setCursortoMiddle(hWnd);
		SetCapture(hWnd);

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
		case VK_CONTROL: {
			//just use left control
			if ((lParam & 0x01000000) == 0) {

			}
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
		camera.perspective(45.0f, static_cast<float>(_width) / static_cast<float>(_height), 1.0f, 5000.0f);

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
		8,								// no alpha buffer
		0,								// ignore shift bit
		0,								// no accumulation buffer
		0, 0, 0, 0,						// ignore accumulation bits
		24,								// 24 bit z-buffer size
		8,								// no stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main drawing plane
		0,								// reserved
		0, 0, 0 };						// layer masks ignored

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);	// choose best matching pixel format
	SetPixelFormat(hDC, nPixelFormat, &pfd);		// set pixel format to device context

													// create rendering context and make it current
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	ReleaseDC(hWnd, hDC);
	enableVerticalSync(true);

	glEnable(GL_DEPTH_TEST);					// hidden surface removal
	//glEnable(GL_CULL_FACE);						// do not calculate inside of poly's

	//setup the camera.
	camera = Camera(Vector3f(0.0f, 0.0f, 20.f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	camera.perspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 1.0f, 5000.0f);

	//setup some meshes
	quad = new MeshQuad(Vector3f(0.0f, 0.0f, 0.f), 1024, 1024, ".\\res\\floor_color_map.png");
	quad->setPrecision(1, 1);
	quad->buildMesh();

	car = new Model();
	car->loadObject("res/car/car.obj");

	wheel[0] = new Model();
	wheel[0]->loadObject("res/wheel/wheel.obj");

	wheel[1] = new Model();
	wheel[1]->loadObject("res/wheel/wheel.obj");

	wheel[2] = new Model();
	wheel[2]->loadObject("res/wheel/wheel.obj");

	wheel[3] = new Model();
	wheel[3]->loadObject("res/wheel/wheel.obj");

	room = new Model();
	room->loadObject("res/room/room.obj", Vector3f(0.0, 0.0, 1.0), 0.0, Vector3f(0.0, 0.0, 0.0), STATICS_SCALE);

	physics = new Physics();

	physics->AddCar(PhysicsUtils::btTransFrom(btVector3(0.2f, 18.95f, 0.7f), btQuaternion(btVector3(0, 1, 0), -PI / 2.f))); // 0,2,5

	physics->AddStaticModel(Physics::CreateStaticCollisionShapes2(room, 1), PhysicsUtils::btTransFrom(btVector3(0, 0, 0)), false, btVector3(1, 1, 1));	
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
	if (pKeyBuffer['W'] & 0xF0) Direction |= DIR_FORWARD;
	if (pKeyBuffer['S'] & 0xF0) Direction |= DIR_BACKWARD;
	if (pKeyBuffer['A'] & 0xF0) Direction |= DIR_LEFT;
	if (pKeyBuffer['D'] & 0xF0) Direction |= DIR_RIGHT;
	if (pKeyBuffer['E'] & 0xF0) Direction |= DIR_UP;
	if (pKeyBuffer['Q'] & 0xF0) Direction |= DIR_DOWN;

	// Now process the mouse (if the button is pressed)
	if (GetCapture() == hWnd) {
		// Hide the mouse pointer
		SetCursor(NULL);
		// Retrieve the cursor position
		GetCursorPos(&CursorPos);

		// Calculate mouse rotational values
		X = (float)(g_OldCursorPos.x - CursorPos.x) * 0.1;
		Y = (float)(g_OldCursorPos.y - CursorPos.y) * 0.1;

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos(g_OldCursorPos.x, g_OldCursorPos.y);

		if (Direction > 0 || X != 0.0f || Y != 0.0f) {
			// Rotate camera
			if (X || Y) {
				camera.rotate(X, Y, 0.0f);

			} // End if any rotation

			if (Direction) {

				float dx = 0, dy = 0, dz = 0, speed = 1.3;

				if (Direction & DIR_FORWARD) dz = speed;
				if (Direction & DIR_BACKWARD) dz = -speed;
				if (Direction & DIR_LEFT) dx = -speed;
				if (Direction & DIR_RIGHT) dx = speed;
				if (Direction & DIR_UP) dy = speed;
				if (Direction & DIR_DOWN) dy = -speed;

				camera.move(dx, dy, dz);
			}

		}// End if any movement
	} // End if Captured
}

void updateFrame(HWND hWnd, double elapsedTimeSec) {

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

	float pitch = 0.0f;
	float heading = 0.0f;

	if (GetCapture() == hWnd) {
		// hide the mouse pointer
		SetCursor(NULL);

		if (Direction) UPDATEFRAME = true;


		if (UPDATEFRAME) {

			if (Direction & DIR_FORWARD) {
				physics->GetCar()->Forward();
			}

			if (Direction & DIR_BACKWARD) {
				physics->GetCar()->Backward();
			}

			if (Direction & DIR_LEFT) {
				physics->GetCar()->TurnLeft();
			}

			if (Direction & DIR_RIGHT) {
				physics->GetCar()->TurnRight();
			}

		}		
	}
}