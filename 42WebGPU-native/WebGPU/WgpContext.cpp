#include <iostream>
#include <Windows.h>
#include "WgpContext.h"

WgpContext wgpContext = {};

void handle_request_adapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPURequestAdapterStatus_Success) {
		struct WgpContext* context = (WgpContext*)userdata1;
		context->adapter = adapter;
	}else {
		std::cout << "ERROR: Adapter" << std::endl;
	}
}

void handle_request_device(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2) {

	if (status == WGPURequestDeviceStatus_Success) {
		struct WgpContext* context = (WgpContext*)userdata1;
		context->device = device;
	}else {
		std::cout << "ERROR: Device" << std::endl;
	}
}

void wgpInit(void* window) {
	wgpCreateDevice(wgpContext, window);
	/*instance = wgpuCreateInstance(NULL);

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

	Queue = wgpuDeviceGetQueue(device);*/
}

bool wgpCreateDevice(WgpContext& wgpContext, void* window) {
	wgpContext.instance = wgpuCreateInstance(NULL);

	WGPUSurfaceSourceWindowsHWND surfaceSourceWindowsHWND = {};
	surfaceSourceWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
	surfaceSourceWindowsHWND.hinstance = GetModuleHandle(NULL);
	surfaceSourceWindowsHWND.hwnd = (HWND)window;

	WGPUSurfaceDescriptor surfaceDescriptor = {};
	surfaceDescriptor.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&surfaceSourceWindowsHWND);

	wgpContext.surface = wgpuInstanceCreateSurface(wgpContext.instance, &surfaceDescriptor);

	WGPURequestAdapterOptions requestAdapterOptions = {};
	requestAdapterOptions.compatibleSurface = wgpContext.surface;

	WGPURequestAdapterCallbackInfo requestAdapterCallbackInfo = {};
	requestAdapterCallbackInfo.callback = handle_request_adapter;
	requestAdapterCallbackInfo.userdata1 = &wgpContext;

	wgpuInstanceRequestAdapter(wgpContext.instance, &requestAdapterOptions, requestAdapterCallbackInfo);

	WGPURequestDeviceCallbackInfo  deviceCallbackInfo = {};
	deviceCallbackInfo.callback = handle_request_device;
	deviceCallbackInfo.userdata1 = &wgpContext;

	wgpuAdapterRequestDevice(wgpContext.adapter, NULL, deviceCallbackInfo);

	wgpContext.queue = wgpuDeviceGetQueue(wgpContext.device);

	return true;
}

WGPUPipelineLayout wgpCreatePipelineLayout() {
	const WGPUDevice& device = wgpContext.device;

	WGPUBindGroupLayoutEntry bglEntry = {};
	bglEntry.binding = 0;
	bglEntry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bglEntry.buffer.type = WGPUBufferBindingType_Uniform;
	bglEntry.buffer.minBindingSize = sizeof(MyUniforms);

	WGPUBindGroupLayoutDescriptor bglDesc = {};
	bglDesc.entryCount = 1;
	bglDesc.entries = &bglEntry;
	wgpContext.bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bglDesc);

	WGPUPipelineLayoutDescriptor layoutDesc = {};
	layoutDesc.bindGroupLayoutCount = 1;
	layoutDesc.bindGroupLayouts = &wgpContext.bindGroupLayout;

	return wgpuDeviceCreatePipelineLayout(device, &layoutDesc);
}

WGPUBuffer wgpCreateBuffer(uint32_t size, WGPUBufferUsage bufferUsage) {
	const WGPUDevice& device = wgpContext.device;
	WGPUBufferDescriptor bufferDesc = {};
	bufferDesc.label = { "buf", WGPU_STRLEN};

	if (bufferUsage & WGPUBufferUsage_Uniform)
		bufferDesc.label = { "uniform_buf" , WGPU_STRLEN};

	if (bufferUsage & WGPUBufferUsage_Vertex)
		bufferDesc.label = { "vertex_buf" , WGPU_STRLEN };

	bufferDesc.size = size;
	bufferDesc.usage = bufferUsage;
	bufferDesc.mappedAtCreation = false;
	return wgpuDeviceCreateBuffer(device, &bufferDesc);
}

WGPUShaderModule load_shader_module(WGPUDevice device, const char* name) {
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
	//assert(buf);
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


WGPUShaderModule WgpContext::createShader(const WGPUDevice& device) {
	return load_shader_module(device, "res/shader/shader.wgsl");
}