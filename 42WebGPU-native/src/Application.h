#pragma once
#include <array>
#include <filesystem>
#include <webgpu.h>
#include <windows.h>
#include <engine/input/Event.h>
#include <engine/input/EventDispatcher.h>
#include <engine/input/Keyboard.h>
#include <engine/input/Mouse.h>
#include <engine/input/GamePad.h>
#include <engine/Vector.h>
#include <States/StateMachine.h>

struct VertexAttributes {
	Vector3f position;
	Vector3f normal;
	Vector3f color;
};

struct MyUniforms {
	Matrix4f projectionMatrix;
	Matrix4f viewMatrix;
	Matrix4f modelMatrix;
	std::array<float, 4> color;
	float time;
	float _pad[3];
};

class Application {

public:
	Application(const float& dt, const float& fdt);
	~Application();

	void update();
	void fixedUpdate();
	void render();
	bool isRunning();

	static void ToggleFullScreen(bool isFullScreen, unsigned int width = 0, unsigned int height = 0);
	static void ToggleVerticalSync();
	static void SetCursorIconFromFile(std::string file);
	static void SetCursorIcon(LPCSTR resource);
	static void SetCursorIcon(HCURSOR cursor);
	static void SetCursorIcon(const char *image[]);
	static const HWND& GetWindow();
	static StateMachine* GetMachine();

	static int Width;
	static int Height;
	static bool OverClient;

	WGPUAdapter adapter;	
	WGPUDevice device;
	WGPUShaderModule load_shader_module(WGPUDevice device, const char* name);

	void setDefault(WGPUDepthStencilState& depthStencilState);
	void setDefault(WGPUBindGroupLayoutEntry& bindingLayout);
	bool loadGeometryFromObj(const std::filesystem::path& path, std::vector<VertexAttributes>& vertexData);

	MyUniforms uniforms;
	float angle1 = 2.0f;
	Matrix4f S;
	Matrix4f T1;
	Matrix4f R1;
	Matrix4f R2;
	Matrix4f T2;

private:
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT ApplicationWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void createWindow();
	void initWebGPU();
	void showWindow();
	void initImGUI();
	void initOpenAL();
	void initStates();

	MSG msg;

	const float& m_fdt;
	const float& m_dt;

	void processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void Resize(int deltaW, int deltaH);
	static StateMachine* Machine;
	static EventDispatcher& EventDispatcher;

	static HWND Window;
	static bool InitWindow;
	static bool Init;
	static DWORD SavedExStyle;
	static DWORD SavedStyle;
	static RECT Savedrc;
	static DEVMODE DefaultScreen;
	static HCURSOR Cursor;
	static HICON Icon;
	static bool VerticalSync;
	static bool Fullscreen;

	
	static WGPUQueue Queue;
	static WGPUSurface Surface;

	WGPUInstance instance;
	WGPURenderPipeline pipeline, pipeline2, render_pipeline;
	WGPUTextureView depthTextureView;
	WGPUBuffer vertexBuffer;
	int indexCount;
	WGPUBuffer uniformBuffer;
	WGPUBindGroup bindGroup;
	//WGPUSurfaceTexture nextTexture;
	
};