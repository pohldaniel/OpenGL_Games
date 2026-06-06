#include <iostream>
#include <Windows.h>
#include "WgpContext.h"
#include "../src/Application.h"

WgpContext wgpContext = {};
std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexAttribute>> wgpVertexAttributes;
std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexBufferLayout>> wgpVertexBufferLayouts;

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
	wgpCreateDevice(window);
}

bool wgpCreateDevice(void* window) {
	

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
	std::vector<WGPUInstanceFeatureName> instancefeatures;
	instancefeatures.push_back(WGPUInstanceFeatureName::WGPUInstanceFeatureName_TimedWaitAny);
	WGPUInstanceDescriptor cinstanceDesc = {};
	cinstanceDesc.requiredFeatureCount = instancefeatures.size();
	cinstanceDesc.requiredFeatures = instancefeatures.data();
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

	std::vector<WGPUFeatureName> deviceFeatures;
	deviceFeatures.push_back(WGPUFeatureName::WGPUFeatureName_PrimitiveIndex);

	WGPUDeviceDescriptor deviceDescriptor = {};
	deviceDescriptor.requiredLimits = &requiredLimits;
	deviceDescriptor.uncapturedErrorCallbackInfo = errorCallbackInfo;
	deviceDescriptor.requiredFeatures = deviceFeatures.data();
	deviceDescriptor.requiredFeatureCount = deviceFeatures.size();

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
	wgpContext.depthTexture = wgpCreateTexture(static_cast<uint32_t>(Application::Width), static_cast<uint32_t>(Application::Height), 1u, WGPUTextureUsage_RenderAttachment, wgpContext.depthformat, 1u, wgpContext.msaaSampleCount, wgpContext.depthformat);
	wgpContext.depthTextureView = wgpCreateTextureView(wgpContext.depthTexture, WGPUTextureAspect::WGPUTextureAspect_All);

	wgpCreateVertexBufferLayout(VL_P);
	wgpCreateVertexBufferLayout(VL_PT);
	wgpCreateVertexBufferLayout(VL_PN);
	wgpCreateVertexBufferLayout(VL_PNC);
	wgpCreateVertexBufferLayout(VL_PTN);
	wgpCreateVertexBufferLayout(VL_PTNC);
	wgpCreateVertexBufferLayout(VL_PTNTB);
	wgpCreateVertexBufferLayout(VL_PTNWJ);	
	wgpCreateVertexBufferLayout(VL_BATCH);	

	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Linear, WGPUAddressMode_ClampToEdge), SS_LINEAR_CLAMP);
	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Linear, WGPUAddressMode_Repeat), SS_LINEAR_REPEAT);
	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Nearest, WGPUAddressMode_ClampToEdge), SS_NEAREST_CLAMP);
	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Nearest, WGPUAddressMode_Repeat), SS_NEAREST_REPEAT);

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

WGPUBuffer wgpCreateEmptyBuffer(uint32_t size, WGPUBufferUsage bufferUsage, bool mappedAtCreation) {
	const WGPUDevice& device = wgpContext.device;
	WGPUBufferDescriptor bufferDesc = {};
	bufferDesc.label = WGPU_STR("buf");

	if (bufferUsage & WGPUBufferUsage_Uniform)
		bufferDesc.label = WGPU_STR("uniform_buf");

	if (bufferUsage & WGPUBufferUsage_Vertex)
		bufferDesc.label = WGPU_STR("vertex_buf");

	if (bufferUsage & WGPUBufferUsage_Index)
		bufferDesc.label = WGPU_STR("index_buf");

	if (bufferUsage & WGPUBufferUsage_Storage)
		bufferDesc.label = WGPU_STR("storage_buf");

	bufferDesc.size = size;
	bufferDesc.usage = bufferUsage;
	bufferDesc.mappedAtCreation = mappedAtCreation;
	return wgpuDeviceCreateBuffer(device, &bufferDesc);
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

	if (bufferUsage & WGPUBufferUsage_Storage)
		bufferDesc.label = WGPU_STR("storage_buf");

	bufferDesc.size = size;
	bufferDesc.usage = bufferUsage;
	bufferDesc.mappedAtCreation = true;

	WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
	void* mapping = wgpuBufferGetMappedRange(buffer, 0, size);
	memcpy(mapping, data, size);
	wgpuBufferUnmap(buffer);
	return buffer;
}

WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, uint32_t depth, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, uint32_t mipLevelCount, uint32_t sampleCount, WGPUTextureFormat viewFormat) {
	const WGPUDevice& device = wgpContext.device;
	WGPUTextureDescriptor textureDescriptor = {};
	textureDescriptor.label = WGPU_STR("texture");
	textureDescriptor.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
	textureDescriptor.size = { width, height, depth };
	textureDescriptor.format = textureFormat;
	textureDescriptor.usage = textureUsage;
	textureDescriptor.mipLevelCount = mipLevelCount;
	textureDescriptor.sampleCount = sampleCount;
	textureDescriptor.nextInChain = NULL;
	if (viewFormat != WGPUTextureFormat_Undefined) {
		textureDescriptor.viewFormatCount = 1u;
		textureDescriptor.viewFormats = &viewFormat;
	}
	return wgpuDeviceCreateTexture(device, &textureDescriptor);
}

WGPUTextureView wgpCreateTextureView(const WGPUTexture& texture, WGPUTextureAspect aspect) {
	WGPUTextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.label = WGPU_STR("texture_view");
	textureViewDescriptor.aspect = aspect;
	textureViewDescriptor.baseArrayLayer = 0u;
	textureViewDescriptor.arrayLayerCount = wgpuTextureGetDepthOrArrayLayers(texture);
	textureViewDescriptor.baseMipLevel = 0u;
	textureViewDescriptor.mipLevelCount = wgpuTextureGetMipLevelCount(texture);
	textureViewDescriptor.dimension = textureViewDescriptor.arrayLayerCount == 6u ? WGPUTextureViewDimension::WGPUTextureViewDimension_Cube : WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	textureViewDescriptor.format = wgpuTextureGetFormat(texture);
	textureViewDescriptor.nextInChain = NULL;
	return wgpuTextureCreateView(texture, &textureViewDescriptor);
}

WGPUSampler wgpCreateSampler(WGPUFilterMode filterMode, WGPUAddressMode addressMode, uint16_t maxAnisotropy, WGPUMipmapFilterMode mipmapFilterMode, WGPUCompareFunction compareFunction) {
	const WGPUDevice& device = wgpContext.device;
	WGPUSamplerDescriptor samplerDescriptor = {};
	samplerDescriptor.label = WGPU_STR("sampler");
	samplerDescriptor.addressModeU = addressMode;
	samplerDescriptor.addressModeV = addressMode;
	samplerDescriptor.addressModeW = addressMode;
	samplerDescriptor.magFilter = filterMode;
	samplerDescriptor.minFilter = filterMode;
	samplerDescriptor.mipmapFilter = mipmapFilterMode == WGPUMipmapFilterMode_Undefined ? ((filterMode == WGPUFilterMode_Nearest) ? WGPUMipmapFilterMode_Nearest : WGPUMipmapFilterMode_Linear) : mipmapFilterMode;
	samplerDescriptor.lodMinClamp = 0.0f;
	samplerDescriptor.lodMaxClamp = 1.0f;
	samplerDescriptor.compare = compareFunction;
	samplerDescriptor.maxAnisotropy = maxAnisotropy;
	return wgpuDeviceCreateSampler(device, &samplerDescriptor);
}

WGPUShaderModule wgpCreateShaderFromFile(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		return NULL;
	}
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	std::string shaderSource(size, ' ');
	file.seekg(0);
	file.read(shaderSource.data(), size);
	file.close();

	WGPUShaderSourceWGSL shaderSourceWGSL = {};
	shaderSourceWGSL.chain.next = NULL;
	shaderSourceWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
	shaderSourceWGSL.code = { shaderSource.c_str(), WGPU_STRLEN };

	WGPUShaderModuleDescriptor shaderModuleDescriptor = {};
	shaderModuleDescriptor.label = { path.c_str(), path.length() };
	shaderModuleDescriptor.nextInChain = &shaderSourceWGSL.chain;

	return wgpuDeviceCreateShaderModule(wgpContext.device, &shaderModuleDescriptor);
}

WGPUShaderModule wgpCreateShaderFromString(std::string strng) {
	WGPUShaderSourceWGSL shaderSourceWGSL = {};
	shaderSourceWGSL.chain.next = NULL;
	shaderSourceWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
	shaderSourceWGSL.code = { strng.c_str(), WGPU_STRLEN };

	WGPUShaderModuleDescriptor shaderModuleDescriptor = {};
	shaderModuleDescriptor.label = WGPU_STR("shader");
	shaderModuleDescriptor.nextInChain = &shaderSourceWGSL.chain;

	return wgpuDeviceCreateShaderModule(wgpContext.device, &shaderModuleDescriptor);
}

void wgpCreateVertexBufferLayout(VertexLayoutSlot slot) {
	if (wgpVertexBufferLayouts.count(VL_P) == 0 && slot == VL_P) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_P];
		wgpVertexAttribute.resize(1);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;
		
		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 3 * sizeof(float);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_P].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_PT) == 0 && slot == VL_PT) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_PT];
		wgpVertexAttribute.resize(2);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;

		wgpVertexAttribute[1].shaderLocation = 1;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 5 * sizeof(float);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_PT].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_PN) == 0 && slot == VL_PN) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_PN];
		wgpVertexAttribute.resize(2);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;

		wgpVertexAttribute[1].shaderLocation = 1;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 6 * sizeof(float);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_PN].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_PNC) == 0 && slot == VL_PNC) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_PNC];
		wgpVertexAttribute.resize(3);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;

		wgpVertexAttribute[1].shaderLocation = 1;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		wgpVertexAttribute[2].shaderLocation = 2;
		wgpVertexAttribute[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x4;
		wgpVertexAttribute[2].offset = 6 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 10 * sizeof(float);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_PNC].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_PTN) == 0 && slot == VL_PTN) {
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
		wgpVertexBufferLayouts[VL_PTN].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_PTNC) == 0 && slot == VL_PTNC) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_PTNC];
		wgpVertexAttribute.resize(4);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;

		wgpVertexAttribute[1].shaderLocation = 1;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		wgpVertexAttribute[2].shaderLocation = 2;
		wgpVertexAttribute[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[2].offset = 5 * sizeof(float);

		wgpVertexAttribute[3].shaderLocation = 3;
		wgpVertexAttribute[3].format = WGPUVertexFormat::WGPUVertexFormat_Float32x4;
		wgpVertexAttribute[3].offset = 8 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 12 * sizeof(float);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_PTNC].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_PTNTB) == 0 && slot == VL_PTNTB) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_PTNTB];
		wgpVertexAttribute.resize(5);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;

		wgpVertexAttribute[1].shaderLocation = 1;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		wgpVertexAttribute[2].shaderLocation = 2;
		wgpVertexAttribute[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[2].offset = 5 * sizeof(float);

		wgpVertexAttribute[3].shaderLocation = 3;
		wgpVertexAttribute[3].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[3].offset = 8 * sizeof(float);

		wgpVertexAttribute[4].shaderLocation = 4;
		wgpVertexAttribute[4].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[4].offset = 11 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 14 * sizeof(float);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_PTNTB].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_PTNWJ) == 0 && slot == VL_PTNWJ) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_PTNWJ];
		wgpVertexAttribute.resize(5);

		wgpVertexAttribute[0].shaderLocation = 0;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0;

		wgpVertexAttribute[1].shaderLocation = 1;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		wgpVertexAttribute[2].shaderLocation = 2;
		wgpVertexAttribute[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[2].offset = 5 * sizeof(float);

		wgpVertexAttribute[3].shaderLocation = 3;
		wgpVertexAttribute[3].format = WGPUVertexFormat::WGPUVertexFormat_Float32x4;
		wgpVertexAttribute[3].offset = 8 * sizeof(float);

		wgpVertexAttribute[4].shaderLocation = 4;
		wgpVertexAttribute[4].format = WGPUVertexFormat::WGPUVertexFormat_Uint32x4;
		wgpVertexAttribute[4].offset = 12 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 12 * sizeof(float) + 4 * sizeof(uint32_t);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_PTNWJ].push_back(wgpVertexBufferLayout);
	}else if (wgpVertexBufferLayouts.count(VL_BATCH) == 0 && slot == VL_BATCH) {
		std::vector<WGPUVertexAttribute>& wgpVertexAttribute = wgpVertexAttributes[VL_BATCH];
		wgpVertexAttribute.resize(4);

		wgpVertexAttribute[0].shaderLocation = 0u;
		wgpVertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
		wgpVertexAttribute[0].offset = 0u;

		wgpVertexAttribute[1].shaderLocation = 1u;
		wgpVertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
		wgpVertexAttribute[1].offset = 3 * sizeof(float);

		wgpVertexAttribute[2].shaderLocation = 2u;
		wgpVertexAttribute[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x4;
		wgpVertexAttribute[2].offset = 5 * sizeof(float);

		wgpVertexAttribute[3].shaderLocation = 3u;
		wgpVertexAttribute[3].format = WGPUVertexFormat::WGPUVertexFormat_Uint32;
		wgpVertexAttribute[3].offset = 9 * sizeof(float);

		WGPUVertexBufferLayout wgpVertexBufferLayout = {};
		wgpVertexBufferLayout.attributeCount = (uint32_t)wgpVertexAttribute.size();
		wgpVertexBufferLayout.attributes = wgpVertexAttribute.data();
		wgpVertexBufferLayout.arrayStride = 9 * sizeof(float) + sizeof(unsigned int);
		wgpVertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;
		wgpVertexBufferLayouts[VL_BATCH].push_back(wgpVertexBufferLayout);
	}
}

std::vector<WGPUVertexAttribute>& wgpVertexAttribute(VertexLayoutSlot vertexLayoutSlot) {
	return wgpVertexAttributes[vertexLayoutSlot];
}

std::vector<WGPUVertexBufferLayout>& wgpVertexBufferLayout(VertexLayoutSlot vertexLayoutSlot) {
	return wgpVertexBufferLayouts[vertexLayoutSlot];
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

	if (wgpContext.msaaTextureView) {
		wgpuTextureViewRelease(wgpContext.msaaTextureView);
		wgpContext.msaaTextureView = NULL;
	}

	if (wgpContext.msaaTexture) {
		wgpuTextureDestroy(wgpContext.msaaTexture);
		wgpuTextureRelease(wgpContext.msaaTexture);
		wgpContext.msaaTexture = NULL;
	}

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

		wgpContext.depthTexture = wgpCreateTexture(width, height, 1u, WGPUTextureUsage_RenderAttachment, wgpContext.depthformat, 1u, wgpContext.msaaSampleCount, wgpContext.depthformat);
		wgpContext.depthTextureView = wgpCreateTextureView(wgpContext.depthTexture, WGPUTextureAspect::WGPUTextureAspect_All);

		if (wgpContext.msaaSampleCount > 1u) {
			wgpuTextureViewRelease(wgpContext.msaaTextureView);
			wgpuTextureDestroy(wgpContext.msaaTexture);
			wgpuTextureRelease(wgpContext.msaaTexture);

			wgpContext.msaaTexture = wgpCreateTexture(width, height, 1u, WGPUTextureUsage_RenderAttachment, wgpContext.colorformat, 1u, wgpContext.msaaSampleCount, wgpContext.colorformat);
			wgpContext.msaaTextureView = wgpCreateTextureView(wgpContext.msaaTexture, WGPUTextureAspect::WGPUTextureAspect_All);
		}

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

void wgpSetSurfaceColorFormat(WGPUTextureFormat textureFormat, const std::function<void()>& onSurfaceChange) {
	if (wgpContext.surface) {
		wgpContext.colorformat = textureFormat;
		wgpContext.config.format = wgpContext.colorformat;
		wgpuSurfaceConfigure(wgpContext.surface, &wgpContext.config);

		if (wgpContext.msaaSampleCount > 1u) {
			uint32_t width = wgpuTextureGetWidth(wgpContext.msaaTexture);
			uint32_t height = wgpuTextureGetHeight(wgpContext.msaaTexture);

			wgpuTextureViewRelease(wgpContext.msaaTextureView);
			wgpuTextureDestroy(wgpContext.msaaTexture);
			wgpuTextureRelease(wgpContext.msaaTexture);

			wgpContext.msaaTexture = wgpCreateTexture(width, height, 1u, WGPUTextureUsage_RenderAttachment, wgpContext.colorformat, 1u, wgpContext.msaaSampleCount, wgpContext.colorformat);
			wgpContext.msaaTextureView = wgpCreateTextureView(wgpContext.msaaTexture, WGPUTextureAspect::WGPUTextureAspect_All);
		}
		if (onSurfaceChange)
			onSurfaceChange();
	}
}

void wgpSetSurfaceDepthFormat(WGPUTextureFormat textureFormat, const std::function<void()>& onSurfaceChange) {
	if (wgpContext.surface) {
		wgpContext.depthformat = textureFormat;

		uint32_t width = wgpuTextureGetWidth(wgpContext.depthTexture);
		uint32_t height = wgpuTextureGetHeight(wgpContext.depthTexture);

		wgpuTextureViewRelease(wgpContext.depthTextureView);
		wgpuTextureDestroy(wgpContext.depthTexture);
		wgpuTextureRelease(wgpContext.depthTexture);

		wgpContext.depthTexture = wgpCreateTexture(width, height, 1u, WGPUTextureUsage_RenderAttachment, wgpContext.depthformat, 1u, wgpContext.msaaSampleCount, wgpContext.depthformat);
		wgpContext.depthTextureView = wgpCreateTextureView(wgpContext.depthTexture, WGPUTextureAspect::WGPUTextureAspect_All);

		if (onSurfaceChange)
			onSurfaceChange();

	}
}

void wgpSetMSAASampleCount(const uint32_t count, const std::function<void()>& onSurfaceChange) {
	if(wgpContext.msaaSampleCount != count){
		wgpContext.msaaSampleCount = count;

		uint32_t width = wgpuTextureGetWidth(wgpContext.depthTexture);
		uint32_t height = wgpuTextureGetHeight(wgpContext.depthTexture);

		if (wgpContext.msaaTexture) {
			wgpuTextureViewRelease(wgpContext.msaaTextureView);
			wgpuTextureDestroy(wgpContext.msaaTexture);
			wgpuTextureRelease(wgpContext.msaaTexture);
		}

		wgpContext.msaaTexture = count == 1u ? NULL : wgpCreateTexture(width, height, 1u, WGPUTextureUsage_RenderAttachment, wgpContext.colorformat, 1u, wgpContext.msaaSampleCount, wgpContext.colorformat);
		wgpContext.msaaTextureView = count == 1u ? NULL : wgpCreateTextureView(wgpContext.msaaTexture, WGPUTextureAspect::WGPUTextureAspect_All);

		wgpuTextureViewRelease(wgpContext.depthTextureView);
		wgpuTextureDestroy(wgpContext.depthTexture);
		wgpuTextureRelease(wgpContext.depthTexture);

		wgpContext.depthTexture = wgpCreateTexture(width, height, 1u, WGPUTextureUsage_RenderAttachment, wgpContext.depthformat, 1u, wgpContext.msaaSampleCount, wgpContext.depthformat);
		wgpContext.depthTextureView = wgpCreateTextureView(wgpContext.depthTexture, WGPUTextureAspect::WGPUTextureAspect_All);

		if (onSurfaceChange)
			onSurfaceChange();
	}
}

WGPURenderPassDepthStencilAttachment wgpCopyDepthStencilAttachment(const WGPURenderPassDepthStencilAttachment* src) {
	WGPURenderPassDepthStencilAttachment dest;
	dest.depthClearValue = src->depthClearValue;
	dest.depthLoadOp = src->depthLoadOp;
	dest.depthReadOnly = src->depthReadOnly;
	dest.depthStoreOp = src->depthStoreOp;

	dest.stencilClearValue = src->stencilClearValue;
	dest.stencilLoadOp = src->stencilLoadOp;
	dest.stencilReadOnly = src->stencilReadOnly;
	dest.stencilStoreOp = src->stencilStoreOp;

	dest.nextInChain = src->nextInChain;
	dest.view = src->view;

	return dest;
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
	renderPassColorAttachment.view = wgpContext.msaaSampleCount == 1u ? texureView  : wgpContext.msaaTextureView;
	renderPassColorAttachment.resolveTarget = wgpContext.msaaSampleCount == 1u ? NULL : texureView;
	renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = wgpContext.clearColor;
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

	WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
	depthStencilAttachment.view = wgpContext.depthTextureView;
	depthStencilAttachment.depthClearValue = 1.0f;
	depthStencilAttachment.depthLoadOp = WGPULoadOp::WGPULoadOp_Clear;
	depthStencilAttachment.depthStoreOp = WGPUStoreOp::WGPUStoreOp_Store;
	depthStencilAttachment.depthReadOnly = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilAttachment.stencilClearValue = 0u;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp::WGPULoadOp_Undefined;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp::WGPUStoreOp_Undefined;
	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool::WGPUOptionalBool_True;

	WGPURenderPassDescriptor renderPassDescriptor = {};
	renderPassDescriptor.colorAttachmentCount = 1u;
	renderPassDescriptor.colorAttachments = &renderPassColorAttachment;
	renderPassDescriptor.depthStencilAttachment = &depthStencilAttachment;
	renderPassDescriptor.timestampWrites = NULL;

	WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
	commandEncoderDescriptor.label = WGPU_STR("command_encoder");
	wgpContext.commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDescriptor);

	wgpContext.OnDraw(wgpContext.commandEncoder, renderPassDescriptor);

	wgpuTextureViewRelease(texureView);

	WGPUCommandBufferDescriptor commandBufferDescriptor = {};
	commandBufferDescriptor.label = WGPU_STR("command_buffer");
	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(wgpContext.commandEncoder, &commandBufferDescriptor);

	wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);

	wgpuSurfacePresent(wgpContext.surface);

	wgpuCommandBufferRelease(commandBuffer);
	wgpuCommandEncoderRelease(wgpContext.commandEncoder);
	wgpuTextureRelease(surfaceTexture.texture);


	if (wgpContext.OnPostDraw)
		wgpContext.OnPostDraw();

#ifdef WEBGPU_DAWN
	wgpuDeviceTick(wgpContext.device);
#endif
	wgpuInstanceProcessEvents(wgpContext.instance);
}

void wgpSubmitQueue() {
	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(wgpContext.commandEncoder, NULL);
	wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);
	wgpuCommandBufferRelease(commandBuffer);
	wgpuCommandEncoderRelease(wgpContext.commandEncoder);

	wgpContext.commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, NULL);
}

void WgpContext::createVertexBufferLayout(VertexLayoutSlot slot) {
	wgpCreateVertexBufferLayout(slot);
}

void WgpContext::addSampler(const WGPUSampler& sampler, SamplerSlot samplerSlot) {
	if (samplers.count(samplerSlot) == 0)
		samplers[samplerSlot] = sampler;
}

const WGPUSampler& WgpContext::getSampler(SamplerSlot samplerSlot) const {
	return samplers.at(samplerSlot);
}

void WgpContext::addSahderModule(const std::string& shaderModuleName, const std::string& stringPath, bool fromString) {
	shaderModules[shaderModuleName] = fromString ? wgpCreateShaderFromString(stringPath) : wgpCreateShaderFromFile(stringPath);
}

const WGPUShaderModule& WgpContext::getShaderModule(std::string shaderModuleName) const {
	return shaderModules.at(shaderModuleName);
}

const WGPUPipelineLayout& WgpContext::getPipelineLayout(std::string pipelineLayoutName) const {
	return pipelineLayouts.at(pipelineLayoutName);
}

void WgpContext::setClearColor(const WGPUColor& _clearColor) {
	clearColor = _clearColor;
}

bool WgpContext::isBlendAble(WGPUTextureFormat textureFormat) {
	return textureFormat != WGPUTextureFormat_R32Uint && textureFormat != WGPUTextureFormat_R32Sint;
}

void WgpContext::createComputePipeline(std::string shaderModuleName, 
	std::string entrypoint,
	std::string pipelineLayoutName, 
	const std::function<std::vector<WGPUBindGroupLayout>()>& onBindGroupLayouts) {

	if (onBindGroupLayouts) {
		std::vector<WGPUBindGroupLayout> bindGroupLayouts = onBindGroupLayouts();
		WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
		pipelineLayoutDescriptor.bindGroupLayoutCount = bindGroupLayouts.size();
		pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
		pipelineLayouts[pipelineLayoutName] = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);
	}

	WGPUComputePipelineDescriptor computePipelineDesc = {};
	computePipelineDesc.layout = onBindGroupLayouts ? pipelineLayouts.at(pipelineLayoutName) : NULL;
	computePipelineDesc.compute.module = shaderModules.at(shaderModuleName);
	computePipelineDesc.compute.entryPoint = { entrypoint.c_str(), entrypoint.size()};
	computePipelineDesc.compute.constantCount = 0u;
	computePipelineDesc.compute.constants = NULL;
	
	computePipelines[pipelineLayoutName] = wgpuDeviceCreateComputePipeline(device, &computePipelineDesc);
}

void WgpContext::createRenderPipeline(std::string shaderModuleName, 
	std::string pipelineLayoutName, 
	const VertexLayoutSlot vertexLayoutSlot, 
	const std::function<std::vector<WGPUBindGroupLayout>()>& onBindGroupLayouts, 
	uint32_t msaaSampleCount, 
	WGPUPrimitiveTopology primitiveTopology,
	WGPUTextureFormat colorTextureFormat,
	WGPUTextureFormat depthTextureFormat,
	WGPUCompareFunction depthCompareFunction,
	const PipelineConfiguration configuration) {

	if (onBindGroupLayouts) {
		std::vector<WGPUBindGroupLayout> bindGroupLayouts = onBindGroupLayouts();
		WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
		pipelineLayoutDescriptor.bindGroupLayoutCount = bindGroupLayouts.size();
		pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
		pipelineLayouts[pipelineLayoutName] = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);
	}	

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModules.at(shaderModuleName);
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = NULL;
	vertexState.bufferCount = vertexLayoutSlot == VertexLayoutSlot::VL_NONE ? 0u : wgpVertexBufferLayouts.at(vertexLayoutSlot).size();
	vertexState.buffers = vertexLayoutSlot == VertexLayoutSlot::VL_NONE ? NULL : wgpVertexBufferLayouts.at(vertexLayoutSlot).data();

	WGPUBlendState blendState = {};
	if (configuration.blendMode == ALPHA_BLENDING) {
		blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
		blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
		blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
		blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;
		blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
		blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	}else if (configuration.blendMode == ADDITIVE_BLENDING_SRC) {
		blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
		blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
		blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
		blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;
		blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
		blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	}else if (configuration.blendMode == ADDITIVE_BLENDING_ONE) {
		blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_One;
		blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
		blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
		blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;
		blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_One;
		blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	}

	std::vector<WGPUColorTargetState> colorTargetStates;
	if (configuration.colorTextureFormat != WGPUTextureFormat_Undefined) {
		colorTargetStates.push_back({ NULL, configuration.colorTextureFormat ,
										   (configuration.flags & BLEND_STATE) && isBlendAble(configuration.colorTextureFormat) ? &blendState : NULL,
											WGPUColorWriteMask_All });
	}

	colorTargetStates.push_back({ NULL, colorTextureFormat == WGPUTextureFormat_Undefined ? colorformat : colorTextureFormat,
									   (configuration.flags & BLEND_STATE) ? &blendState : NULL,
										WGPUColorWriteMask_All });
	
	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModules.at(shaderModuleName);
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = configuration.constantEntries.size();
	fragmentState.constants = configuration.constantEntries.empty() ? NULL : configuration.constantEntries.data();
	fragmentState.targetCount = colorTargetStates.size();
	fragmentState.targets = colorTargetStates.data();

	WGPUDepthStencilState depthStencilState = {};
	setDefault(depthStencilState);

	if (configuration.stencilMode == StencilMode::SET) {
		depthStencilState.stencilFront.passOp = WGPUStencilOperation_Replace;
	}
	
	if (configuration.stencilMode == StencilMode::MASK) {
		depthStencilState.stencilFront.compare = WGPUCompareFunction_Equal;
		depthStencilState.stencilBack.compare = WGPUCompareFunction_Equal;
	}

	depthStencilState.depthCompare = depthCompareFunction;
	depthStencilState.depthWriteEnabled = (configuration.flags & WRITE_DEPTH) ? WGPUOptionalBool::WGPUOptionalBool_True : WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilState.format = depthTextureFormat == WGPUTextureFormat_Undefined ? depthformat : depthTextureFormat;
	depthStencilState.stencilReadMask = (configuration.stencilMode == StencilMode::SET || configuration.stencilMode == StencilMode::MASK) ? 255u : 0u;
	depthStencilState.stencilWriteMask = (configuration.stencilMode == StencilMode::SET || configuration.stencilMode == StencilMode::MASK) ? 255u : 0u;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = onBindGroupLayouts ? pipelineLayouts.at(pipelineLayoutName) : NULL;
	renderPipelineDescriptor.multisample.count = msaaSampleCount;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = (configuration.flags & FRAGMENT_STATE) ? &fragmentState : NULL;
	renderPipelineDescriptor.depthStencil = (configuration.flags & DEPTH_STENCIL_STATE) ? &depthStencilState : NULL;

	renderPipelineDescriptor.primitive.topology = primitiveTopology;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = configuration.cullMode == WGPUCullMode_Undefined ? WGPUCullMode::WGPUCullMode_Back : configuration.cullMode;

	renderPipelines[pipelineLayoutName] = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDescriptor);
}