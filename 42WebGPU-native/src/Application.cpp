#include <iostream>
#include <time.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <engine/sound/SoundDevice.h>
#include <engine/ui/Widget.h>
#include <engine/Framebuffer.h>
#include <engine/Fontrenderer.h>
#include <engine/DebugRenderer.h>
#include <engine/Sprite.h>
#include <engine/utils/Utils.h>

#include <States/Menu.h>
#include <States/Default.h>
#include <States/Game.h>
#include <States/Controller.h>

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
bool Application::OverClient = true;
bool Application::VerticalSync = true;

HCURSOR Application::Cursor = LoadCursor(nullptr, IDC_ARROW);
HICON Application::Icon = (HICON)LoadImage(NULL, "res/poison.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

WGPUQueue Application::Queue;
WGPUSurface Application::Surface;

Application::Application(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	Width = WIDTH;
	Height = HEIGHT;

	createWindow();
	showWindow();
	initWebGPU();
	
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

	SetCursorIcon(IDC_ARROW);
	initStates();
}

Application::~Application() {
	delete Machine;
	
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
		"Super Pirate",
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
		case WM_MOUSELEAVE: {
			OverClient = false;

			POINT cursorPos;
			GetCursorPos(&cursorPos);
			ScreenToClient(Window, &cursorPos);

			Event event;
			event.type = Event::MOUSEMOTION;
			event.data.mouseMove.x = cursorPos.x;
			event.data.mouseMove.y = cursorPos.y;

			event.data.mouseMove.button = wParam & MK_RBUTTON ? Event::MouseMoveEvent::MouseButton::BUTTON_RIGHT : wParam & MK_LBUTTON ? Event::MouseMoveEvent::MouseButton::BUTTON_LEFT : Event::MouseMoveEvent::MouseButton::NONE;
			EventDispatcher.pushEvent(event);
			break;
		}case WM_CREATE: {
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

	/*if ((message == WM_KEYDOWN && (wParam == 'v' || wParam == 'V' || wParam == 'z' || wParam == 'Z')) || (message == WM_KEYDOWN && wParam == VK_ESCAPE) || (message == WM_KEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN))) || (message == WM_SYSKEYDOWN && wParam == VK_RETURN && ((HIWORD(lParam) & KF_ALTDOWN)))) {
		ImGui::GetIO().WantCaptureMouse = false;
	}

	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	if (InitWindow && ImGui::GetIO().WantCaptureMouse) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}*/

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

void handle_request_adapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPURequestAdapterStatus_Success) {
		struct Application* application = (Application*)userdata1;
		application->adapter = adapter;
	}else {
		std::cout << "ERROR: Adapter" << std::endl;
	}
}

void handle_request_device(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2) {

		if (status == WGPURequestDeviceStatus_Success) {
			struct Application* application = (Application*)userdata1;
			application->device = device;
		}else {
			std::cout << "ERROR: Device" << std::endl;
		}
}

WGPUShaderModule Application::load_shader_module(WGPUDevice device, const char* name) {
	FILE* file = NULL;
	char* buf = NULL;
	WGPUShaderModule shader_module = NULL;

	file = fopen(name, "rb");
	if (!file) {
		perror("fopen");
		goto cleanup;
	}

	if (fseek(file, 0, SEEK_END) != 0) {
		perror("fseek");
		goto cleanup;
	}
	long length = ftell(file);
	if (length == -1) {
		perror("ftell");
		goto cleanup;
	}
	if (fseek(file, 0, SEEK_SET) != 0) {
		perror("fseek");
		goto cleanup;
	}

	buf = (char*)malloc(length + 1);
	assert(buf);
	fread(buf, 1, length, file);
	buf[length] = 0;

	WGPUShaderSourceWGSL shaderSourceWGSL = {};
	shaderSourceWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
	shaderSourceWGSL.code = { buf, WGPU_STRLEN };

	WGPUShaderModuleDescriptor shaderModuleDescriptor = {};
	shaderModuleDescriptor.label = { name, WGPU_STRLEN };
	shaderModuleDescriptor.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&shaderSourceWGSL);

	shader_module = wgpuDeviceCreateShaderModule(device, &shaderModuleDescriptor);

cleanup:
	if (file)
		fclose(file);
	if (buf)
		free(buf);
	return shader_module;
}

void Application::setDefault(WGPUDepthStencilState& depthStencilState) {
	depthStencilState.nextInChain = nullptr;
	depthStencilState.format = WGPUTextureFormat::WGPUTextureFormat_Undefined;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_True;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

	//depthStencilState.stencilFront = WGPUStencilFaceState::compare;
	//depthStencilState.stencilBack = WGPUStencilFaceState::WGPUCompareFunction;

	depthStencilState.stencilReadMask = 0;
	depthStencilState.stencilWriteMask = 0;

	depthStencilState.depthBias = 0;

	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;
}

void Application::setDefault(WGPUBindGroupLayoutEntry& bindingLayout) {
	bindingLayout.buffer.nextInChain = nullptr;
	bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
	bindingLayout.buffer.hasDynamicOffset = false;

	bindingLayout.sampler.nextInChain = nullptr;
	bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

	bindingLayout.storageTexture.nextInChain = nullptr;
	bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
	bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
	bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

	bindingLayout.texture.nextInChain = nullptr;
	bindingLayout.texture.multisampled = false;
	bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
	bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;
}

bool Application::loadGeometryFromObj(const std::filesystem::path& path, std::vector<VertexAttributes>& vertexData) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	// Call the core loading procedure of TinyOBJLoader
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str());

	// Check errors
	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		return false;
	}

	// Filling in vertexData:
	vertexData.clear();
	for (const auto& shape : shapes) {
		size_t offset = vertexData.size();
		vertexData.resize(offset + shape.mesh.indices.size());

		for (size_t i = 0; i < shape.mesh.indices.size(); ++i) {
			const tinyobj::index_t& idx = shape.mesh.indices[i];

			vertexData[offset + i].position = {
				attrib.vertices[3 * idx.vertex_index + 0],
				-attrib.vertices[3 * idx.vertex_index + 2], // Add a minus to avoid mirroring
				attrib.vertices[3 * idx.vertex_index + 1]
			};

			// Also apply the transform to normals!!
			vertexData[offset + i].normal = {
				attrib.normals[3 * idx.normal_index + 0],
				-attrib.normals[3 * idx.normal_index + 2],
				attrib.normals[3 * idx.normal_index + 1]
			};

			vertexData[offset + i].color = {
				attrib.colors[3 * idx.vertex_index + 0],
				attrib.colors[3 * idx.vertex_index + 1],
				attrib.colors[3 * idx.vertex_index + 2]
			};
		}
	}

	return true;
}

void Application::initWebGPU() {
	instance = wgpuCreateInstance(NULL);

	WGPUSurfaceSourceWindowsHWND surfaceSourceWindowsHWND = {};
	surfaceSourceWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
	surfaceSourceWindowsHWND.hinstance = GetModuleHandle(NULL);
	surfaceSourceWindowsHWND.hwnd = Window;

	WGPUSurfaceDescriptor surfaceDescriptor = {};
	surfaceDescriptor.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&surfaceSourceWindowsHWND);

	Surface = wgpuInstanceCreateSurface(instance, &surfaceDescriptor);

	WGPURequestAdapterOptions requestAdapterOptions = {};
	requestAdapterOptions.compatibleSurface = Surface;

	WGPURequestAdapterCallbackInfo requestAdapterCallbackInfo = {};
	requestAdapterCallbackInfo.callback = handle_request_adapter;
	requestAdapterCallbackInfo.userdata1 = this;

	wgpuInstanceRequestAdapter(instance, &requestAdapterOptions, requestAdapterCallbackInfo);

	WGPURequestDeviceCallbackInfo  deviceCallbackInfo = {};
	deviceCallbackInfo.callback = handle_request_device;
	deviceCallbackInfo.userdata1 = this;

	wgpuAdapterRequestDevice(adapter, NULL, deviceCallbackInfo);
	assert(device);

	Queue = wgpuDeviceGetQueue(device);
	assert(Queue);

	WGPUShaderModule shader_module = load_shader_module(device, "res/shader/triangle.wgsl");
	assert(shader_module);

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.label = { "pipeline_layout", WGPU_STRLEN };

	WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);
	assert(pipeline_layout);

	WGPUSurfaceCapabilities surface_capabilities = { 0 };
	wgpuSurfaceGetCapabilities(Surface, adapter, &surface_capabilities);

	WGPUVertexState vertexState = {};
	vertexState.module = shader_module;
	vertexState.entryPoint = { "vs_main", WGPU_STRLEN };

	WGPUColorTargetState colorTargetState = {};
	colorTargetState.format = surface_capabilities.formats[0];
	colorTargetState.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shader_module;
	fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTargetState;

	WGPUMultisampleState multisampleState = {};
	multisampleState.count = 1;
	multisampleState.mask = 0xFFFFFFFF;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.label = { "render_pipeline", WGPU_STRLEN };
	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.multisample = multisampleState;

	render_pipeline = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDescriptor);

	WGPUSurfaceConfiguration surfaceConfiguration = {};
	surfaceConfiguration.device = device;
	surfaceConfiguration.usage = WGPUTextureUsage_RenderAttachment;
	surfaceConfiguration.format = surface_capabilities.formats[0];
	surfaceConfiguration.presentMode = WGPUPresentMode_Fifo;
	surfaceConfiguration.alphaMode = surface_capabilities.alphaModes[0];
	surfaceConfiguration.width = Application::Width;
	surfaceConfiguration.height = Application::Height;


	wgpuSurfaceConfigure(Surface, &surfaceConfiguration);

	/*WGPURequestAdapterOptions adapterOptions = {};
	adapterOptions.compatibleSurface = Surface;

	WGPURequestAdapterCallbackInfo adapterCallbackInfo = {};
	adapterCallbackInfo.userdata1 = this;
	adapterCallbackInfo.callback = handle_request_adapter;
	wgpuInstanceRequestAdapter(instance, &adapterOptions, adapterCallbackInfo);


	WGPURequestDeviceCallbackInfo deviceCallbackInfo = {};
	deviceCallbackInfo.userdata1 = this;
	deviceCallbackInfo.callback = handle_request_device;
	wgpuAdapterRequestDevice(adapter, NULL, deviceCallbackInfo);

	Queue = wgpuDeviceGetQueue(device);

	CreateRenderPipeline();

	std::cout << "Creating shader module..." << std::endl;
	WGPUShaderModule shaderModule = load_shader_module(device, "res/shader/shader.wgsl");
	std::cout << "Shader module: " << shaderModule << std::endl;
	std::vector<WGPUVertexAttribute> vertexAttribs(3);

	vertexAttribs[0].shaderLocation = 0;
	vertexAttribs[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
	vertexAttribs[0].offset = 0;

	vertexAttribs[1].shaderLocation = 1;
	vertexAttribs[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
	vertexAttribs[1].offset = offsetof(VertexAttributes, normal);

	vertexAttribs[2].shaderLocation = 2;
	vertexAttribs[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
	vertexAttribs[2].offset = offsetof(VertexAttributes, color);

	WGPUVertexBufferLayout vertexBufferLayout;
	vertexBufferLayout.attributeCount = (uint32_t)vertexAttribs.size();
	vertexBufferLayout.attributes = vertexAttribs.data();
	vertexBufferLayout.arrayStride = sizeof(VertexAttributes);
	vertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;

	WGPURenderPipelineDescriptor pipelineDesc;
	pipelineDesc.vertex.bufferCount = 1;
	pipelineDesc.vertex.buffers = &vertexBufferLayout;

	pipelineDesc.vertex.module = shaderModule;
	pipelineDesc.vertex.entryPoint = { "vs_main", WGPU_STRLEN };
	pipelineDesc.vertex.constantCount = 0;
	pipelineDesc.vertex.constants = nullptr;

	pipelineDesc.primitive.topology = WGPUPrimitiveTopology::WGPUPrimitiveTopology_TriangleList;
	pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	pipelineDesc.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	pipelineDesc.primitive.cullMode = WGPUCullMode::WGPUCullMode_Front;

	WGPUFragmentState fragmentState;
	pipelineDesc.fragment = &fragmentState;
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
	fragmentState.constantCount = 0;
	fragmentState.constants = nullptr;

	WGPUBlendState blendState;
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;

	WGPUColorTargetState colorTarget;
	colorTarget.format = WGPUTextureFormat::WGPUTextureFormat_BGRA8UnormSrgb;
	colorTarget.blend = &blendState;
	colorTarget.writeMask = WGPUColorWriteMask_All;

	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;

	WGPUDepthStencilState depthStencilState = {};
	setDefault(depthStencilState);
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool_True;

	WGPUTextureFormat depthTextureFormat = WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;
	depthStencilState.format = depthTextureFormat;
	depthStencilState.stencilReadMask = 0;
	depthStencilState.stencilWriteMask = 0;

	pipelineDesc.depthStencil = &depthStencilState;

	pipelineDesc.multisample.count = 1;
	pipelineDesc.multisample.mask = ~0u;
	pipelineDesc.multisample.alphaToCoverageEnabled = false;

	// bind group layout (used by both the pipeline layout and uniform bind group, released at the end of this function)
	WGPUBindGroupLayoutEntry bglEntry = {};
	bglEntry.binding = 0;
	bglEntry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bglEntry.buffer.type = WGPUBufferBindingType_Uniform;
	bglEntry.buffer.minBindingSize = sizeof(MyUniforms);

	WGPUBindGroupLayoutDescriptor bglDesc = {};
	bglDesc.entryCount = 1;
	bglDesc.entries = &bglEntry;
	WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bglDesc);

	WGPUPipelineLayoutDescriptor layoutDesc = {};
	layoutDesc.bindGroupLayoutCount = 1;
	layoutDesc.bindGroupLayouts = &bindGroupLayout;
	WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &layoutDesc);
	pipelineDesc.layout = pipelineLayout;

	WGPURenderPipelineDescriptor desc = {};

	desc.layout = pipelineLayout;

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = { "vs_main", WGPU_STRLEN };
	vertexState.constantCount = 0;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 1;
	vertexState.buffers = &vertexBufferLayout;

	WGPUBlendState _blendState;
	_blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	_blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	_blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	_blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;
	_blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	_blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;

	WGPUColorTargetState _colorTarget;
	_colorTarget.format = WGPUTextureFormat::WGPUTextureFormat_BGRA8UnormSrgb;
	_colorTarget.blend = &_blendState;
	_colorTarget.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState _fragmentState = {};
	_fragmentState.module = shaderModule;
	_fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
	_fragmentState.constantCount = 0;
	_fragmentState.constants = nullptr;
	_fragmentState.targetCount = 1;
	_fragmentState.targets = &_colorTarget;

	WGPUDepthStencilState _depthStencilState = {};
	setDefault(_depthStencilState);
	_depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;
	_depthStencilState.depthWriteEnabled = WGPUOptionalBool_True;

	WGPUTextureFormat _depthTextureFormat = WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;
	_depthStencilState.format = _depthTextureFormat;
	_depthStencilState.stencilReadMask = 0;
	_depthStencilState.stencilWriteMask = 0;
	
	desc.multisample.count = 1;
	desc.multisample.mask = ~0u;
	desc.multisample.alphaToCoverageEnabled = false;

	desc.vertex = vertexState;
	desc.fragment = &_fragmentState;
	desc.depthStencil = &depthStencilState;

	pipeline = wgpuDeviceCreateRenderPipeline(device, &desc);
	std::cout << "Render pipeline: " << pipeline << std::endl;

	WGPUTextureDescriptor depthTextureDesc = {};
	depthTextureDesc.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
	depthTextureDesc.format = _depthTextureFormat;
	depthTextureDesc.mipLevelCount = 1;
	depthTextureDesc.sampleCount = 1;
	depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;
	depthTextureDesc.size = { static_cast<uint32_t>(Application::Width), static_cast<uint32_t>(Application::Height), 1 };
	depthTextureDesc.viewFormatCount = 1;
	depthTextureDesc.viewFormats = (WGPUTextureFormat*)&_depthTextureFormat;
	WGPUTexture depthTexture = wgpuDeviceCreateTexture(device, &depthTextureDesc);
	std::cout << "Depth texture: " << depthTexture << std::endl;

	WGPUTextureViewDescriptor depthTextureViewDesc = {};
	depthTextureViewDesc.aspect = WGPUTextureAspect::WGPUTextureAspect_DepthOnly;
	depthTextureViewDesc.baseArrayLayer = 0;
	depthTextureViewDesc.arrayLayerCount = 1;
	depthTextureViewDesc.baseMipLevel = 0;
	depthTextureViewDesc.mipLevelCount = 1;
	depthTextureViewDesc.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	depthTextureViewDesc.format = depthTextureFormat;
	depthTextureView = wgpuTextureCreateView(depthTexture, &depthTextureViewDesc);
	std::cout << "Depth texture view: " << depthTextureView << std::endl;

	std::vector<float> pointData;
	std::vector<uint16_t> indexData;

	// Load mesh data from OBJ file
	std::vector<VertexAttributes> vertexData;
	bool success = loadGeometryFromObj("res/models/mammoth.obj", vertexData);
	if (!success) {
		std::cerr << "Could not load geometry!" << std::endl;
		return;
	}

	// Create vertex buffer
	WGPUBufferDescriptor bufferDesc = {};
	bufferDesc.label = { "vertex_buf", WGPU_STRLEN };
	bufferDesc.size = vertexData.size() * sizeof(VertexAttributes);
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
	bufferDesc.mappedAtCreation = false;
	vertexBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
	wgpuQueueWriteBuffer(Queue, vertexBuffer, 0, vertexData.data(), bufferDesc.size);
	indexCount = static_cast<int>(vertexData.size());

	// Create uniform buffer
	bufferDesc.label = { "uniform_buf", WGPU_STRLEN };
	bufferDesc.size = sizeof(MyUniforms);
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
	bufferDesc.mappedAtCreation = false;
	uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
	
	// Translate the view
	Vector3f focalPoint(0.0, 0.0, -1.0);
	float angle2 = 3.0f * PI / 4.0f;
	S = Matrix4f::Scale(0.3f, 0.3f, 0.3f);
	T1 = Matrix4f::IDENTITY;
	R1 = Matrix4f::Rotate(0.0f, 0.0f, angle1);
	uniforms.modelMatrix = R1 * T1 * S;

	R2 = Matrix4f::Rotate(-angle2, 0.0f, 0.0f);
	T2 = Matrix4f::Translate(-focalPoint);
	uniforms.viewMatrix = T2 * R2;

	float ratio = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	float focalLength = 2.0;
	float _near = 0.01f;
	float _far = 100.0f;
	float divider = 1 / (focalLength * (_far - _near));
	uniforms.projectionMatrix = Matrix4f::Transpose(Matrix4f(
		1.0, 0.0, 0.0, 0.0,
		0.0, ratio, 0.0, 0.0,
		0.0, 0.0, _far * divider, -_far * _near * divider,
		0.0, 0.0, 1.0 / focalLength, 0.0
	));

	uniforms.time = 1.0f;
	uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	wgpuQueueWriteBuffer(Queue, uniformBuffer, 0, &uniforms, sizeof(MyUniforms));


	WGPUBindGroupEntry binding = {};
	binding.binding = 0;
	binding.buffer = uniformBuffer;
	binding.offset = 0;
	binding.size = sizeof(MyUniforms);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = bindGroupLayout;
	bindGroupDesc.entryCount = bglDesc.entryCount;
	bindGroupDesc.entries = &binding;
	bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDesc);

	// Configure the surface
	WGPUSurfaceConfiguration config = {};
	config.nextInChain = nullptr;

	// Configuration of the textures created for the underlying swap chain
	config.width = Width;
	config.height = Height;
	config.usage = WGPUTextureUsage_RenderAttachment;

	WGPUSurfaceCapabilities surface_capabilities = { 0 };
	wgpuSurfaceGetCapabilities(Surface, adapter, &surface_capabilities);

	WGPUTextureFormat surfaceFormat = surface_capabilities.formats[0];
	config.format = surfaceFormat;

	// And we do not need any particular view format:
	config.viewFormatCount = 0;
	config.viewFormats = nullptr;
	config.device = device;
	config.presentMode = WGPUPresentMode_Fifo;
	config.alphaMode = WGPUCompositeAlphaMode_Auto;

	wgpuSurfaceConfigure(Surface, &config);

	// Release the adapter only after it has been fully utilized
	wgpuAdapterRelease(adapter);*/
}

void Application::initImGUI() {
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window);
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplOpenGL3_Init("#version 410 core");
}

void Application::initOpenAL() {
	SoundDevice::init();
}

void Application::ToggleVerticalSync() {

	// WGL_EXT_swap_control.
	typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(GLint);
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

const char shaderCode[] = R"(
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        return vec4f(pos[i], 0, 1);
    }
    @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
    }
)";

void Application::CreateRenderPipeline() {
	WGPUShaderSourceWGSL wgsl = {};
	wgsl.chain.next = nullptr;
	wgsl.chain.sType = WGPUSType_ShaderSourceWGSL;
	wgsl.code.data = shaderCode;
	wgsl.code.length = WGPU_STRLEN;

	WGPUShaderModuleDescriptor shaderModuleDescriptor = {};
	shaderModuleDescriptor.nextInChain = (const WGPUChainedStruct*)&wgsl;

	WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDescriptor);

	WGPUSurfaceCapabilities surface_capabilities = {};
	wgpuSurfaceGetCapabilities(Surface, adapter, &surface_capabilities);
	WGPUTextureFormat surfaceFormat = surface_capabilities.formats[0];

	WGPUColorTargetState colorTarget = {};
	colorTarget.format = surfaceFormat;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = { "fragmentMain", WGPU_STRLEN };
	fragmentState.constantCount = 0;
	fragmentState.constants = nullptr;
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;

	WGPURenderPipelineDescriptor desc = {};
	desc.vertex.module = shaderModule;
	desc.vertex.entryPoint = { "vertexMain", WGPU_STRLEN };
	desc.fragment = &fragmentState;
	desc.multisample.count = 1;

	pipeline2 = wgpuDeviceCreateRenderPipeline(device, &desc);

	WGPUSurfaceConfiguration config = {};
	config.nextInChain = nullptr;
	config.width = Width;
	config.height = Height;
	config.usage = WGPUTextureUsage_RenderAttachment;
	config.format = surfaceFormat;
	config.viewFormatCount = 0;
	config.viewFormats = nullptr;
	config.device = device;
	config.presentMode = WGPUPresentMode_Fifo;
	config.alphaMode = WGPUCompositeAlphaMode_Auto;

	wgpuSurfaceConfigure(Surface, &config);
}

bool tmp = false;
void Application::render() {

	/*uniforms.time = static_cast<float>(Globals::clock.getElapsedTimeSec());
	wgpuQueueWriteBuffer(Queue, uniformBuffer, offsetof(MyUniforms, time), &uniforms.time, sizeof(MyUniforms::time));
	angle1 = uniforms.time;
	R1 = Matrix4f::Rotate(0.0f, 0.0f, angle1);
	uniforms.modelMatrix = R1 * T1 * S;
	wgpuQueueWriteBuffer(Queue, uniformBuffer, offsetof(MyUniforms, modelMatrix), &uniforms.modelMatrix, sizeof(MyUniforms::modelMatrix));
	wgpuSurfaceGetCurrentTexture(Surface, &nextTexture);
	
	WGPUTextureView frame = wgpuTextureCreateView(nextTexture.texture, NULL);

	WGPUCommandEncoderDescriptor commandEncoderDesc = {};
	commandEncoderDesc.label = { "command_encoder", WGPU_STRLEN };
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &commandEncoderDesc);

	WGPURenderPassDescriptor renderPassDesc = {};

	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.view = frame;
	renderPassColorAttachment.resolveTarget = nullptr;
	renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = WGPUColor{ 0.2, 0.2, 0.2, 1.0 };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &renderPassColorAttachment;

	WGPURenderPassDepthStencilAttachment depthStencilAttachment;
	depthStencilAttachment.view = depthTextureView;
	depthStencilAttachment.depthClearValue = 1.0f;
	depthStencilAttachment.depthLoadOp = WGPULoadOp::WGPULoadOp_Clear;
	depthStencilAttachment.depthStoreOp = WGPUStoreOp::WGPUStoreOp_Store;
	depthStencilAttachment.depthReadOnly = false;
	depthStencilAttachment.stencilClearValue = 0;

	depthStencilAttachment.stencilLoadOp = WGPULoadOp::WGPULoadOp_Undefined;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp::WGPUStoreOp_Undefined;
	depthStencilAttachment.stencilReadOnly = true;

	renderPassDesc.depthStencilAttachment = &depthStencilAttachment;
	renderPassDesc.timestampWrites = nullptr;

	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

	wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
	wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, wgpuBufferGetSize(vertexBuffer));
	wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);
	wgpuRenderPassEncoderDraw(renderPass, indexCount, 1, 0, 0);
	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);
	wgpuTextureViewRelease(frame);
	

	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	cmdBufferDescriptor.label = { "Command buffer", WGPU_STRLEN };
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);

	wgpuCommandEncoderRelease(encoder);
	wgpuQueueSubmit(Queue, 1, &command);
	wgpuCommandBufferRelease(command);

	wgpuTextureRelease(nextTexture.texture);*/

	/*if (!tmp) {
		wgpuSurfaceGetCurrentTexture(Surface, &nextTexture);
		tmp = true;
	}
	WGPUTextureView frame = wgpuTextureCreateView(nextTexture.texture, NULL);

	WGPURenderPassDescriptor renderPassDesc = {};
	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.view = frame;
	renderPassColorAttachment.resolveTarget = nullptr;
	renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = WGPUColor{ 0.2, 0.2, 0.2, 1.0 };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &renderPassColorAttachment;

	WGPUCommandEncoderDescriptor commandEncoderDesc = {};
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &commandEncoderDesc);
	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
	wgpuRenderPassEncoderSetPipeline(renderPass, pipeline2);
	wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);
	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);

	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);

	wgpuCommandEncoderRelease(encoder);
	wgpuQueueSubmit(Queue, 1, &command);
	wgpuCommandBufferRelease(command);*/

	WGPUSurfaceTexture surface_texture;
	wgpuSurfaceGetCurrentTexture(Surface, &surface_texture);
	switch (surface_texture.status) {
		case WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal:
		case WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal:
			// All good, could handle suboptimal here
			break;
		case WGPUSurfaceGetCurrentTextureStatus_Timeout:
		case WGPUSurfaceGetCurrentTextureStatus_Outdated:
		case WGPUSurfaceGetCurrentTextureStatus_Lost: {
			// Skip this frame, and re-configure surface.
			if (surface_texture.texture != NULL) {
				wgpuTextureRelease(surface_texture.texture);
			}			
			break;
		}	
	}
	assert(surface_texture.texture);

	WGPUTextureView frame = wgpuTextureCreateView(surface_texture.texture, NULL);
	assert(frame);

	WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
	commandEncoderDescriptor.label = { "command_encoder", WGPU_STRLEN };

	WGPUCommandEncoder command_encoder = wgpuDeviceCreateCommandEncoder(device, &commandEncoderDescriptor);
	assert(command_encoder);

	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.view = frame;
	renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	renderPassColorAttachment.clearValue = { 0.0f, 1.0f, 0.0f, 1.0f };

	WGPURenderPassDescriptor renderPassDescriptor = {};
	renderPassDescriptor.label = { "render_pass_encoder", WGPU_STRLEN };
	renderPassDescriptor.colorAttachmentCount = 1;
	renderPassDescriptor.colorAttachments = &renderPassColorAttachment;

	WGPURenderPassEncoder render_pass_encoder =
		wgpuCommandEncoderBeginRenderPass(command_encoder, &renderPassDescriptor);
	assert(render_pass_encoder);

	wgpuRenderPassEncoderSetPipeline(render_pass_encoder, render_pipeline);
	wgpuRenderPassEncoderDraw(render_pass_encoder, 3, 1, 0, 0);
	wgpuRenderPassEncoderEnd(render_pass_encoder);
	wgpuRenderPassEncoderRelease(render_pass_encoder);

	WGPUCommandBufferDescriptor commandBufferDescriptor = {};
	commandBufferDescriptor.label = { "command_buffer", WGPU_STRLEN };

	WGPUCommandBuffer command_buffer = wgpuCommandEncoderFinish(command_encoder, &commandBufferDescriptor);
	assert(command_buffer);

	wgpuQueueSubmit(Queue, 1, &command_buffer);
	wgpuSurfacePresent(Surface);

	wgpuCommandBufferRelease(command_buffer);
	wgpuCommandEncoderRelease(command_encoder);
	wgpuTextureViewRelease(frame);
	wgpuTextureRelease(surface_texture.texture);
}

void Application::update() {
	Mouse::instance().update();
	Keyboard::instance().update();
	XInputController::instance().update();
	
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
	//Machine->addStateAtTop(new Game(*Machine));
	Machine->addStateAtTop(new Controller(*Machine));

}

void Application::processEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
		case WM_MOUSEMOVE: {

			if (!OverClient) {
				OverClient = true;
				SetCursor(Cursor);
			}

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
		}case WM_MBUTTONDOWN: {
			Event event;
			event.type = Event::MOUSEBUTTONDOWN;
			event.data.mouseButton.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
			event.data.mouseButton.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
			event.data.mouseButton.button = Event::MouseButtonEvent::MouseButton::BUTTON_MIDDLE;
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
			case VK_CONTROL: {

				//Alt Graph
				//if (HIWORD(lParam) == 29)
				//	break;

				Event event;
				event.type = Event::KEYDOWN;
				event.data.keyboard.keyCode = (lParam & 0x01000000) != 0 ? VK_RCONTROL : VK_LCONTROL;
				EventDispatcher.pushEvent(event);
				break;
			}
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

			Event event;
			event.type = Event::KEYDOWN;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);

			break;
		}case WM_KEYUP: {

			switch (wParam) {
				case VK_CONTROL: {
					Event event;
					event.type = Event::KEYUP;
					event.data.keyboard.keyCode = (lParam & 0x01000000) != 0 ? VK_RCONTROL : VK_LCONTROL;
					EventDispatcher.pushEvent(event);
					break;
				}
			}

			Event event;
			event.type = Event::KEYUP;
			event.data.keyboard.keyCode = wParam;
			EventDispatcher.pushEvent(event);

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
		Sprite::Resize(Width, Height);
		auto shader = Globals::shaderManager.getAssetPointer("font_ttf");
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

//https://learn.microsoft.com/en-us/windows/win32/menurc/using-cursors
void Application::SetCursorIcon(const char* image[]) {
	int i, row, col;
	BYTE  data[4 * 32];
	BYTE  mask[4 * 32];
	int hot_x, hot_y;
	i = -1;
	for (row = 0; row < 32; ++row) {
		for (col = 0; col < 32; ++col) {
			if (col % 8) {
				data[i] <<= 1;
				mask[i] <<= 1;
			}else {
				++i;
				data[i] = mask[i] = 0;
			}

			switch (image[4 + row][col]) {
				case '.':
					data[i] |= 0x00;
					break;
				case '+':
					mask[i] |= 0x01;
					break;
				case ' ':
					data[i] |= 0x01;
					break;
			}
		}
	}
	sscanf(image[4 + row], "%d,%d", &hot_x, &hot_y);
	Application::Cursor = CreateCursor(NULL, hot_x, hot_y, 32, 32, data, mask);

	SetCursor(Cursor);
	SetClassLongPtr(Window, GCLP_HCURSOR, LONG_PTR(Cursor));
}