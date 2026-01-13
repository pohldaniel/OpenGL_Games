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

void setDefault(WGPULimits& limits) {
	limits.maxTextureDimension1D = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxTextureDimension2D = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxTextureDimension3D = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxTextureArrayLayers = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBindGroups = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBindGroupsPlusVertexBuffers = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBindingsPerBindGroup = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxDynamicUniformBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxDynamicStorageBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxSampledTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxSamplersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxStorageBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxStorageTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxUniformBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxUniformBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED;
	limits.maxStorageBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED;
	limits.minUniformBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED;
	limits.minStorageBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxVertexBuffers = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBufferSize = WGPU_LIMIT_U64_UNDEFINED;
	limits.maxVertexAttributes = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxVertexBufferArrayStride = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxInterStageShaderVariables = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxColorAttachments = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxColorAttachmentBytesPerSample = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupStorageSize = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeInvocationsPerWorkgroup = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupSizeX = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupSizeY = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupSizeZ = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupsPerDimension = WGPU_LIMIT_U32_UNDEFINED;	
}

void wgpInit(void* window) {
	wgpCreateDevice(wgpContext, window);
}

bool wgpCreateDevice(WgpContext& wgpContext, void* window) {
	wgpContext.instance = wgpuCreateInstance(NULL);

	WGPUSurfaceSourceWindowsHWND surfaceSourceWindowsHWND = {};
	surfaceSourceWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
	surfaceSourceWindowsHWND.chain.next = nullptr;
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

	
	WGPULimits requiredLimits = {};
	setDefault(requiredLimits);
	requiredLimits.maxTextureDimension1D = 2048;
	requiredLimits.maxTextureDimension2D = 2048;
	requiredLimits.maxTextureDimension3D = 2048;

	WGPUDeviceDescriptor deviceDescriptor = {};
	deviceDescriptor.requiredLimits = &requiredLimits;
	
	wgpuAdapterRequestDevice(wgpContext.adapter, &deviceDescriptor, deviceCallbackInfo);

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

WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, WGPUTextureFormat textureFormat, WGPUTextureUsage textureUsage) {
	const WGPUDevice& device = wgpContext.device;

	WGPUTextureDescriptor textureDescriptor = {};
	textureDescriptor.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
	textureDescriptor.format = textureFormat;
	textureDescriptor.mipLevelCount = 1;
	textureDescriptor.sampleCount = 1;
	textureDescriptor.usage = textureUsage;
	textureDescriptor.size = { width, height, 1 };
	textureDescriptor.viewFormatCount = 1;
	textureDescriptor.nextInChain = nullptr;
	textureDescriptor.viewFormats = &wgpContext.depthformat;
	return wgpuDeviceCreateTexture(device, &textureDescriptor);;
}

WGPUTextureView wgpCreateTextureView(WGPUTextureFormat textureFormat, const WGPUTexture& texture) {
	WGPUTextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.aspect = WGPUTextureAspect::WGPUTextureAspect_DepthOnly;
	textureViewDescriptor.baseArrayLayer = 0;
	textureViewDescriptor.arrayLayerCount = 1;
	textureViewDescriptor.baseMipLevel = 0;
	textureViewDescriptor.mipLevelCount = 1;
	textureViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	textureViewDescriptor.format = textureFormat;
	textureViewDescriptor.nextInChain = nullptr;
	return wgpuTextureCreateView(texture, &textureViewDescriptor);
}

void vlkDraw() {

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