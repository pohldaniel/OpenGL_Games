#include <iostream>
#include <Windows.h>
#include "WgpContext.h"
#include "WgpTexture.h"
#include "../src/Application.h"

WgpContext wgpContext = {};
std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexAttribute>> wgpVertexAttributes;
std::unordered_map<VertexLayoutSlot, WGPUVertexBufferLayout> wgpVertexBufferLayouts;

void OnRequestAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPURequestAdapterStatus_Success) {
		struct WgpContext* context = (WgpContext*)userdata1;
		context->adapter = adapter;
	}else {
		std::cout << "Adapter message: " << message.data << std::endl;
	}
}

void OnRequestDevice(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2) {

	if (status == WGPURequestDeviceStatus_Success) {
		struct WgpContext* context = (WgpContext*)userdata1;
		context->device = device;
	}else {
		std::cout << "Device message: " << message.data << std::endl;
	}
}

void OnErrorDevice(const WGPUDevice* device, WGPUErrorType type, WGPUStringView message, void* userdata1, void* userdata2) {
	std::cout << "Error: " << type << " - message: " << message.data << "\n";
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

void setDefault(WGPUStencilFaceState& stencilFaceState) {
	stencilFaceState.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	stencilFaceState.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	stencilFaceState.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	stencilFaceState.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
}

void setDefault(WGPUDepthStencilState& depthStencilState) {
	depthStencilState.nextInChain = NULL;
	depthStencilState.format = WGPUTextureFormat::WGPUTextureFormat_Undefined;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_True;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;
	setDefault(depthStencilState.stencilFront);
	setDefault(depthStencilState.stencilBack);
	depthStencilState.stencilReadMask = 0;
	depthStencilState.stencilWriteMask = 0;
	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;
}

void setDefault(WGPUBindGroupLayoutEntry& bindingLayout) {
	bindingLayout.buffer.nextInChain = NULL;
	bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
	bindingLayout.buffer.hasDynamicOffset = false;

	bindingLayout.sampler.nextInChain = NULL;
	bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

	bindingLayout.storageTexture.nextInChain = NULL;
	bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
	bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
	bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

	bindingLayout.texture.nextInChain = NULL;
	bindingLayout.texture.multisampled = false;
	bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
	bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;
}

void wgpInit(void* window) {
	wgpCreateDevice(wgpContext, window);
}

bool wgpCreateDevice(WgpContext& wgpContext, void* window) {
	

#ifdef WEBGPU_NATIVE
	WGPUInstanceExtras instanceExtras = { };
	instanceExtras.chain.sType = (WGPUSType)WGPUSType_InstanceExtras;
	instanceExtras.chain.next = NULL;
	instanceExtras.backends = WGPUInstanceBackend_Primary;

	WGPUInstanceDescriptor instanceDescriptor = {};
	//instanceDescriptor.nextInChain = &instanceExtras.chain;
	//instanceDescriptor.features.
	wgpContext.instance = wgpuCreateInstance(&instanceDescriptor);
#else	
	static const WGPUInstanceFeatureName ckTimedWaitAny = WGPUInstanceFeatureName::WGPUInstanceFeatureName_TimedWaitAny;
	WGPUInstanceDescriptor cinstanceDesc = {};
	cinstanceDesc.requiredFeatureCount = 1;
	cinstanceDesc.requiredFeatures = &ckTimedWaitAny;
	wgpContext.instance = wgpuCreateInstance(&cinstanceDesc);
#endif
	
	
	WGPUSurfaceSourceWindowsHWND surfaceSourceWindowsHWND = {};
	surfaceSourceWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
	surfaceSourceWindowsHWND.chain.next = NULL;
	surfaceSourceWindowsHWND.hinstance = GetModuleHandle(NULL);
	surfaceSourceWindowsHWND.hwnd = (HWND)window;
	
	WGPUSurfaceDescriptor surfaceDescriptor = {};
	surfaceDescriptor.nextInChain = &surfaceSourceWindowsHWND.chain;

	WGPURequestAdapterOptions requestAdapterOptions = {};
	requestAdapterOptions.compatibleSurface = wgpContext.surface;
	requestAdapterOptions.forceFallbackAdapter = false;
	requestAdapterOptions.powerPreference = WGPUPowerPreference_HighPerformance;
	requestAdapterOptions.backendType = WGPUBackendType_Vulkan;

	WGPURequestAdapterCallbackInfo requestAdapterCallbackInfo = {};
	requestAdapterCallbackInfo.callback = OnRequestAdapter;
	requestAdapterCallbackInfo.userdata1 = &wgpContext;
	requestAdapterCallbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
	WGPUFuture futureAdapter = wgpuInstanceRequestAdapter(wgpContext.instance, &requestAdapterOptions, requestAdapterCallbackInfo);

#ifndef WEBGPU_NATIVE
	WGPUFutureWaitInfo waitAdapter = {};
	waitAdapter.future = futureAdapter;
	WGPUWaitStatus statusAdapter = wgpuInstanceWaitAny(wgpContext.instance, 1, &waitAdapter, 0);
#endif

	WGPURequestDeviceCallbackInfo  deviceCallbackInfo = {};
	deviceCallbackInfo.callback = OnRequestDevice;
	deviceCallbackInfo.userdata1 = &wgpContext;
	deviceCallbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;

	WGPULimits requiredLimits = {};
	setDefault(requiredLimits);
	requiredLimits.maxTextureDimension1D = 2048;
	requiredLimits.maxTextureDimension2D = 2048;
	requiredLimits.maxTextureDimension3D = 2048;
	requiredLimits.maxSamplersPerShaderStage = 1;

	WGPUUncapturedErrorCallbackInfo errorCallbackInfo = {};
	errorCallbackInfo.callback = OnErrorDevice;

	WGPUDeviceDescriptor deviceDescriptor = {};
	deviceDescriptor.requiredLimits = &requiredLimits;
	deviceDescriptor.uncapturedErrorCallbackInfo = errorCallbackInfo;

	WGPUFuture futureDevice = wgpuAdapterRequestDevice(wgpContext.adapter, &deviceDescriptor, deviceCallbackInfo);

#ifndef WEBGPU_NATIVE
	WGPUFutureWaitInfo waitDevice = {};
	waitDevice.future = futureDevice;
	WGPUWaitStatus statusDevice = wgpuInstanceWaitAny(wgpContext.instance, 1, &waitDevice, 0);
#endif

	wgpContext.surface = wgpuInstanceCreateSurface(wgpContext.instance, &surfaceDescriptor);
	wgpContext.surfaceCapabilities = { 0 };
	wgpuSurfaceGetCapabilities(wgpContext.surface, wgpContext.adapter, &wgpContext.surfaceCapabilities);
	//wgpContext.colorformat = wgpContext.surfaceCapabilities.formats[0];

	wgpContext.queue = wgpuDeviceGetQueue(wgpContext.device);
	wgpContext.depthTexture = wgpCreateTexture(static_cast<uint32_t>(Application::Width), static_cast<uint32_t>(Application::Height), WGPUTextureUsage_RenderAttachment, wgpContext.depthformat, wgpContext.depthformat);
	wgpContext.depthTextureView = wgpCreateTextureView(wgpContext.depthformat, WGPUTextureAspect::WGPUTextureAspect_DepthOnly, wgpContext.depthTexture);

	wgpCreateVertexBufferLayout();

	return true;
}

void wgpConfigureSurface() {
	wgpContext.config = {};
	wgpContext.config.nextInChain = NULL;
	wgpContext.config.format = wgpContext.colorformat;
	wgpContext.config.width = Application::Width;
	wgpContext.config.height = Application::Height;
	wgpContext.config.usage = WGPUTextureUsage_RenderAttachment;
	wgpContext.config.viewFormatCount = 0;
	wgpContext.config.viewFormats = NULL;
	wgpContext.config.device = wgpContext.device;
	wgpContext.config.presentMode = WGPUPresentMode_Fifo;
	wgpContext.config.alphaMode = wgpContext.surfaceCapabilities.alphaModes[0];
	wgpuSurfaceConfigure(wgpContext.surface, &wgpContext.config);
}

WGPUBuffer wgpCreateBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage) {
	const WGPUDevice& device = wgpContext.device;
	WGPUBufferDescriptor bufferDesc = {};
	bufferDesc.label = WGPU_STR("buf");

	if (bufferUsage & WGPUBufferUsage_Uniform)
		bufferDesc.label = WGPU_STR("uniform_buf");

	if (bufferUsage & WGPUBufferUsage_Vertex)
		bufferDesc.label = WGPU_STR("vertex_buf");

	if (bufferUsage & WGPUBufferUsage_Index)
		bufferDesc.label = WGPU_STR("index_buf");

	bufferDesc.size = size;
	bufferDesc.usage = bufferUsage;
	bufferDesc.mappedAtCreation = true;

	WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
	void* mapping = wgpuBufferGetMappedRange(buffer, 0, size);
	memcpy(mapping, data, size);
	wgpuBufferUnmap(buffer);
	return buffer;
}

WGPUBuffer wgpCreateEmptyBuffer(uint32_t size, WGPUBufferUsage bufferUsage) {
	const WGPUDevice& device = wgpContext.device;
	WGPUBufferDescriptor bufferDesc = {};
	bufferDesc.label = WGPU_STR("buf");

	if (bufferUsage & WGPUBufferUsage_Uniform)
		bufferDesc.label = WGPU_STR("uniform_buf");

	if (bufferUsage & WGPUBufferUsage_Vertex)
		bufferDesc.label = WGPU_STR("vertex_buf");

	if (bufferUsage & WGPUBufferUsage_Index)
		bufferDesc.label = WGPU_STR("index_buf");

	bufferDesc.size = size;
	bufferDesc.usage = bufferUsage;
	bufferDesc.mappedAtCreation = false;
	return wgpuDeviceCreateBuffer(device, &bufferDesc);
}

WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, WGPUTextureFormat viewFormat) {
	const WGPUDevice& device = wgpContext.device;
	WGPUTextureDescriptor textureDescriptor = {};
	textureDescriptor.label = WGPU_STR("texture");
	textureDescriptor.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
	textureDescriptor.size = { width, height, 1 };
	textureDescriptor.format = textureFormat;
	textureDescriptor.usage = textureUsage;
	textureDescriptor.mipLevelCount = 1;
	textureDescriptor.sampleCount = 1;	
	textureDescriptor.nextInChain = NULL;
	if (viewFormat != WGPUTextureFormat_Undefined) {
		textureDescriptor.viewFormatCount = 1;
		textureDescriptor.viewFormats = &viewFormat;
	}
	return wgpuDeviceCreateTexture(device, &textureDescriptor);
}

WGPUTextureView wgpCreateTextureView(WGPUTextureFormat textureFormat, WGPUTextureAspect aspect, const WGPUTexture& texture) {
	WGPUTextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.label = WGPU_STR("texture_view");
	textureViewDescriptor.aspect = aspect;
	textureViewDescriptor.baseArrayLayer = 0;
	textureViewDescriptor.arrayLayerCount = 1;
	textureViewDescriptor.baseMipLevel = 0;
	textureViewDescriptor.mipLevelCount = 1;
	textureViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	textureViewDescriptor.format = textureFormat;
	textureViewDescriptor.nextInChain = NULL;
	return wgpuTextureCreateView(texture, &textureViewDescriptor);
}

WGPUSampler wgpCreateSampler() {
	const WGPUDevice& device = wgpContext.device;
	WGPUSamplerDescriptor samplerDescriptor = {};
	samplerDescriptor.label = WGPU_STR("texture_view");
	samplerDescriptor.addressModeU = WGPUAddressMode_ClampToEdge;
	samplerDescriptor.addressModeV = WGPUAddressMode_ClampToEdge;
	samplerDescriptor.addressModeW = WGPUAddressMode_ClampToEdge;
	samplerDescriptor.magFilter = WGPUFilterMode_Linear;
	samplerDescriptor.minFilter = WGPUFilterMode_Linear;
	samplerDescriptor.mipmapFilter = WGPUMipmapFilterMode_Linear;
	samplerDescriptor.lodMinClamp = 0.0f;
	samplerDescriptor.lodMaxClamp = 1.0f;
	samplerDescriptor.compare = WGPUCompareFunction_Undefined;
	samplerDescriptor.maxAnisotropy = 1;
	return wgpuDeviceCreateSampler(device, &samplerDescriptor);
}

WGPUShaderModule wgpCreateShader(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		return NULL;
	}
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	std::string shaderSource(size, ' ');
	file.seekg(0);
	file.read(shaderSource.data(), size);

	WGPUShaderSourceWGSL shaderSourceWGSL = {};
	shaderSourceWGSL.chain.next = NULL;
	shaderSourceWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
	shaderSourceWGSL.code = { shaderSource.c_str(), WGPU_STRLEN };

	WGPUShaderModuleDescriptor shaderModuleDescriptor;
	shaderModuleDescriptor.nextInChain = &shaderSourceWGSL.chain;
#ifdef WEBGPU_BACKEND_WGPU
	shaderDesc.hintCount = 0;
	shaderDesc.hints = NULL;
#endif
	return wgpuDeviceCreateShaderModule(wgpContext.device, &shaderModuleDescriptor);

	/*const WGPUDevice& device = wgpContext.device;
	FILE* file = NULL;
	char* buf = NULL;
	WGPUShaderModule shaderModule = NULL;

	file = fopen(path.c_str(), "rb");
	if (!file) {
		perror("fopen");
		goto cleanup;
	}

	if (fseek(file, 0, SEEK_END) != 0) {
		perror("fseek");
		goto cleanup;
	}
	unsigned long long length = ftell(file);
	if (length == -1) {
		perror("ftell");
		goto cleanup;
	}
	if (fseek(file, 0, SEEK_SET) != 0) {
		perror("fseek");
		goto cleanup;
	}

	buf = (char*)malloc(length + 1);
	fread(buf, 1, length, file);
	buf[length] = 0;

	WGPUShaderSourceWGSL shaderSourceWGSL = {};
	shaderSourceWGSL.chain.next = NULL;
	shaderSourceWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
	shaderSourceWGSL.code = { buf, WGPU_STRLEN };
	WGPUShaderModuleDescriptor shaderModuleDescriptor = {};
	shaderModuleDescriptor.label = { path.c_str(), path.length() };
	shaderModuleDescriptor.nextInChain = &shaderSourceWGSL.chain;

	shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDescriptor);

cleanup:
	if (file)
		fclose(file);
	if (buf)
		free(buf);
	return shaderModule;*/
}

void wgpCreateVertexBufferLayout(VertexLayoutSlot slot) {
	if (wgpVertexBufferLayouts.count(VL_PTN) == 0 && slot == VL_PTN) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_PTN];
		wgpVertexAttribute.resize(3);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;

		wgpVertexAttribute[1].shaderLocation = 1;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		wgpVertexAttribute[2].shaderLocation = 2;
		wgpVertexAttribute[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[2].offset = 5 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 8 * sizeof(float);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts.emplace(VL_PTN, wgpVertexBufferLayout);
	}
}

void wgpPipelineLayoutsRelease() {
	for (auto& it : wgpContext.pipelineLayouts) {
		wgpuPipelineLayoutRelease(it.second);
	}

	wgpContext.pipelineLayouts.clear();
	wgpContext.pipelineLayouts.rehash(0u);
}

void wgpPipelinesRelease() {
	for (auto& it : wgpContext.renderPipelines) {
		WGPUBindGroupLayout bindGroupLayout = wgpuRenderPipelineGetBindGroupLayout(it.second, 0);
		wgpuBindGroupLayoutRelease(bindGroupLayout);
		wgpuRenderPipelineRelease(it.second);
	}

	wgpContext.renderPipelines.clear();
	wgpContext.renderPipelines.rehash(0u);

	for (auto& it : wgpContext.computePipelines) {
		WGPUBindGroupLayout bindGroupLayout = wgpuComputePipelineGetBindGroupLayout(it.second, 0);
		wgpuBindGroupLayoutRelease(bindGroupLayout);
		wgpuComputePipelineRelease(it.second);
	}

	wgpContext.computePipelines.clear();
	wgpContext.computePipelines.rehash(0u);
}

void wgpSamplersRelease() {
	for (auto& it : wgpContext.samplers) {
		wgpuSamplerRelease(it.second);
	}

	wgpContext.samplers.clear();
	wgpContext.samplers.rehash(0u);
}

void wgpShaderModulesRelease() {
	for (auto& it : wgpContext.shaderModules) {
		wgpuShaderModuleRelease(it.second);
	}

	wgpContext.shaderModules.clear();
	wgpContext.shaderModules.rehash(0u);
}

void wgpShutDown() {
	wgpPipelineLayoutsRelease();
	wgpPipelinesRelease();
	wgpSamplersRelease();
	wgpShaderModulesRelease();

	wgpuTextureViewRelease(wgpContext.depthTextureView);
	wgpContext.depthTextureView = NULL;

	wgpuTextureDestroy(wgpContext.depthTexture);
	wgpuTextureRelease(wgpContext.depthTexture);
	wgpContext.depthTexture = NULL;

	wgpuQueueRelease(wgpContext.queue);
	wgpContext.queue = NULL;

	wgpuAdapterRelease(wgpContext.adapter);
	wgpContext.adapter = NULL;

	wgpuSurfaceRelease(wgpContext.surface);
	wgpContext.surface = NULL;

	wgpuInstanceRelease(wgpContext.instance);
	wgpContext.instance = NULL;

	wgpuDeviceDestroy(wgpContext.device);
	wgpuDeviceRelease(wgpContext.device);
	wgpContext.device = NULL;
}

void wgpResize(uint32_t width, uint32_t height) {
	if (wgpContext.surface) {
		wgpuTextureViewRelease(wgpContext.depthTextureView);
		wgpuTextureDestroy(wgpContext.depthTexture);
		wgpuTextureRelease(wgpContext.depthTexture);

		wgpContext.depthTexture = wgpCreateTexture(width, height, WGPUTextureUsage_RenderAttachment, WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, WGPUTextureFormat::WGPUTextureFormat_Depth24Plus);
		wgpContext.depthTextureView = wgpCreateTextureView(WGPUTextureFormat_Depth24Plus, WGPUTextureAspect::WGPUTextureAspect_DepthOnly, wgpContext.depthTexture);

		wgpContext.config.width = width;
		wgpContext.config.height = height;
		wgpuSurfaceConfigure(wgpContext.surface, &wgpContext.config);
	}
}

void wgpToggleVerticalSync() {
	if (wgpContext.surface) {
		wgpContext.config.presentMode = wgpContext.config.presentMode == WGPUPresentMode_Fifo ? WGPUPresentMode_Immediate : WGPUPresentMode_Fifo;
		wgpuSurfaceConfigure(wgpContext.surface, &wgpContext.config);
	}
}

void wgpDraw() {
	WGPUSurfaceTexture surfaceTexture;
	wgpuSurfaceGetCurrentTexture(wgpContext.surface, &surfaceTexture);
	switch (surfaceTexture.status) {
		case WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal:
		case WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal:
			break;
		case WGPUSurfaceGetCurrentTextureStatus_Timeout:
		case WGPUSurfaceGetCurrentTextureStatus_Outdated:
		case WGPUSurfaceGetCurrentTextureStatus_Lost: {
			if (surfaceTexture.texture != NULL) {
				wgpuTextureRelease(surfaceTexture.texture);
				wgpuSurfaceConfigure(wgpContext.surface, &wgpContext.config);
			}
			return;
		}
	}

	WGPUTextureView texureView = wgpuTextureCreateView(surfaceTexture.texture, NULL);

	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.view = texureView;
	renderPassColorAttachment.resolveTarget = NULL;
	renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = WGPUColor{ 0.2f, 0.2f, 0.2f, 1.0f };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	
	WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
	depthStencilAttachment.view = wgpContext.depthTextureView;
	depthStencilAttachment.depthClearValue = 1.0f;
	depthStencilAttachment.depthLoadOp = WGPULoadOp::WGPULoadOp_Clear;
	depthStencilAttachment.depthStoreOp = WGPUStoreOp::WGPUStoreOp_Store;
	depthStencilAttachment.depthReadOnly = false;
	depthStencilAttachment.stencilClearValue = 0u;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp::WGPULoadOp_Undefined;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp::WGPUStoreOp_Undefined;
	depthStencilAttachment.stencilReadOnly = true;

	WGPURenderPassDescriptor renderPassDesc = {};
	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &renderPassColorAttachment;
	renderPassDesc.depthStencilAttachment = &depthStencilAttachment;
	renderPassDesc.timestampWrites = NULL;

	WGPUCommandEncoderDescriptor commandEncoderDesc = {};
	commandEncoderDesc.label = WGPU_STR("command_encoder");
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDesc);
	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
	wgpContext.OnDraw(renderPass);

	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);
	wgpuTextureViewRelease(texureView);

	WGPUCommandBufferDescriptor commandBufferDescriptor = {};
	commandBufferDescriptor.label = WGPU_STR("command_buffer");
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);
	wgpuQueueSubmit(wgpContext.queue, 1, &command );

	wgpuSurfacePresent(wgpContext.surface);
#ifdef WEBGPU_DAWN
	wgpuDeviceTick(wgpContext.device);
#endif
	wgpuInstanceProcessEvents(wgpContext.instance);


	wgpuCommandBufferRelease(command);
	wgpuCommandEncoderRelease(encoder);
	
	wgpuTextureRelease(surfaceTexture.texture);
}

void WgpContext::createVertexBufferLayout(VertexLayoutSlot slot) {
	wgpCreateVertexBufferLayout(slot);
}

void WgpContext::addSampler(const WGPUSampler& sampler, SamplerSlot samplerSlot) {
	if (samplers.count(samplerSlot) == 0)
		samplers[samplerSlot] = sampler;
}

const WGPUSampler& WgpContext::getSampler(SamplerSlot samplerSlot) {
	return samplers.at(samplerSlot);
}

void WgpContext::addSahderModule(const std::string& shaderModuleName, const std::string& shaderModulePath) {
	shaderModules[shaderModuleName] = wgpCreateShader(shaderModulePath);
}

const WGPUShaderModule& WgpContext::getShaderModule(std::string shaderModuleName) {
	return shaderModules.at(shaderModuleName);
}

void WgpContext::createRenderPipelinePTN(std::string shaderModuleName, std::function <WGPUBindGroupLayout()> onBindGroupLayout) {
	WGPUBindGroupLayout bindGroupLayout = onBindGroupLayout();

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
	pipelineLayoutDescriptor.bindGroupLayouts = &bindGroupLayout;
	pipelineLayouts["RP_PTN"] = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModules.at(shaderModuleName);
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0;
	vertexState.constants = NULL;
	vertexState.bufferCount = 1;
	vertexState.buffers = &wgpVertexBufferLayouts.at(VL_PTN);

	WGPUBlendState blendState = {};
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;

	WGPUColorTargetState colorTarget = {};
	colorTarget.format = colorformat;
	colorTarget.blend = &blendState;
	colorTarget.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModules.at(shaderModuleName);
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;

	WGPUDepthStencilState depthStencilState = {};
	setDefault(depthStencilState);
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool_True;
	depthStencilState.format = wgpContext.depthformat;
	depthStencilState.stencilReadMask = 0;
	depthStencilState.stencilWriteMask = 0;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayouts.at("RP_PTN");
	renderPipelineDescriptor.multisample.count = 1;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = false;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology::WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode::WGPUCullMode_Back;

	wgpContext.renderPipelines[RP_PTN] = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);
}

void WgpContext::createRenderPipelineWireframe(std::string shaderModuleName, std::function <WGPUBindGroupLayout()> onBindGroupLayout) {
	WGPUBindGroupLayout bindGroupLayout = onBindGroupLayout();

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
	pipelineLayoutDescriptor.bindGroupLayouts = &bindGroupLayout;
	pipelineLayouts["RP_WIREFRAME"] = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModules.at(shaderModuleName);
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0;
	vertexState.constants = NULL;

	WGPUBlendState blendState = {};
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;

	WGPUColorTargetState colorTarget = {};
	colorTarget.format = colorformat;
	colorTarget.blend = &blendState;
	colorTarget.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModules.at(shaderModuleName);
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;

	WGPUDepthStencilState depthStencilState = {};
	setDefault(depthStencilState);
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool_True;
	depthStencilState.format = wgpContext.depthformat;
	depthStencilState.stencilReadMask = 0;
	depthStencilState.stencilWriteMask = 0;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayouts.at("RP_WIREFRAME");
	renderPipelineDescriptor.multisample.count = 1;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = false;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology::WGPUPrimitiveTopology_LineList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode::WGPUCullMode_None;

	wgpContext.renderPipelines[RP_WIREFRAME] = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);
}

void WgpContext::createComputePipeline(std::string shaderModuleName, std::string pipelineLayoutName, std::function <WGPUBindGroupLayout()> onBindGroupLayout) {
	WGPUBindGroupLayout bindGroupLayout = onBindGroupLayout();

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
	pipelineLayoutDescriptor.bindGroupLayouts = &bindGroupLayout;
	pipelineLayouts[pipelineLayoutName] = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUComputePipelineDescriptor computePipelineDesc = {};
	computePipelineDesc.layout = pipelineLayouts.at(pipelineLayoutName);
	computePipelineDesc.compute.module = shaderModules.at(shaderModuleName);
	//computePipelineDesc.compute.entryPoint = WGPU_STR("computeFilter");
	computePipelineDesc.compute.entryPoint = WGPU_STR("computeSobelX");
	computePipelineDesc.compute.constantCount = 0;
	computePipelineDesc.compute.constants = NULL;
	
	wgpContext.computePipelines[pipelineLayoutName] = wgpuDeviceCreateComputePipeline(wgpContext.device, &computePipelineDesc);
}