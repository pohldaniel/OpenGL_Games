#include <iostream>
#include <Windows.h>
#include "WgpContext.h"
#include "WgpTexture.h"
#include "../src/Application.h"
#include "../src/Globals.h"

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

void setDefault(WGPUDepthStencilState& depthStencilState) {
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

void setDefault(WGPUBindGroupLayoutEntry& bindingLayout) {
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
	requiredLimits.maxSamplersPerShaderStage = 1;

	WGPUDeviceDescriptor deviceDescriptor = {};
	deviceDescriptor.requiredLimits = &requiredLimits;
	//deviceDescriptor.requiredFeatures

	wgpuAdapterRequestDevice(wgpContext.adapter, &deviceDescriptor, deviceCallbackInfo);

	wgpContext.queue = wgpuDeviceGetQueue(wgpContext.device);

	WGPUShaderModule shaderModule = wgpContext.createShader(wgpContext.device);

	std::vector<WGPUVertexAttribute> vertexAttribs(3);
	vertexAttribs[0].shaderLocation = 0;
	vertexAttribs[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
	vertexAttribs[0].offset = 0;

	vertexAttribs[1].shaderLocation = 1;
	vertexAttribs[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
	vertexAttribs[1].offset = 3 * sizeof(float);

	vertexAttribs[2].shaderLocation = 2;
	vertexAttribs[2].format = WGPUVertexFormat::WGPUVertexFormat_Float32x3;
	vertexAttribs[2].offset = 5 * sizeof(float);

	WGPUVertexBufferLayout vertexBufferLayout;
	vertexBufferLayout.attributeCount = (uint32_t)vertexAttribs.size();
	vertexBufferLayout.attributes = vertexAttribs.data();
	vertexBufferLayout.arrayStride = 8 * sizeof(float);
	vertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;

	WGPUPipelineLayout pipelineLayout = wgpCreatePipelineLayout();

	WGPURenderPipelineDescriptor desc = {};
	desc.layout = pipelineLayout;

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = { "vs_main", WGPU_STRLEN };
	vertexState.constantCount = 0;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 1;
	vertexState.buffers = &vertexBufferLayout;

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

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = { "fs_main", WGPU_STRLEN };
	fragmentState.constantCount = 0;
	fragmentState.constants = nullptr;
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;

	WGPUDepthStencilState depthStencilState = {};
	setDefault(depthStencilState);
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool_True;

	depthStencilState.format = WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;
	depthStencilState.stencilReadMask = 0;
	depthStencilState.stencilWriteMask = 0;

	desc.multisample.count = 1;
	desc.multisample.mask = ~0u;
	desc.multisample.alphaToCoverageEnabled = false;

	desc.vertex = vertexState;
	desc.fragment = &fragmentState;
	desc.depthStencil = &depthStencilState;

	desc.primitive.topology = WGPUPrimitiveTopology::WGPUPrimitiveTopology_TriangleList;
	desc.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	desc.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	desc.primitive.cullMode = WGPUCullMode::WGPUCullMode_Back;

	wgpContext.pipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &desc);
	wgpContext.depthTexture = wgpCreateTexture(static_cast<uint32_t>(Application::Width), static_cast<uint32_t>(Application::Height), WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, WGPUTextureUsage_RenderAttachment);
	wgpContext.depthTextureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, WGPUTextureAspect::WGPUTextureAspect_DepthOnly, wgpContext.depthTexture);

	wgpContext.uniformBuffer = wgpCreateBuffer(sizeof(MyUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	Vector3f focalPoint(0.0, 0.0, -1.0);
	float angle2 = 3.0f * PI / 4.0f;
	wgpContext.S = Matrix4f::Scale(0.3f, 0.3f, 0.3f);
	wgpContext.T1 = Matrix4f::IDENTITY;
	wgpContext.R1 = Matrix4f::Rotate(0.0f, 0.0f, wgpContext.angle1 * _180_ON_PI);
	wgpContext.uniforms.modelMatrix = wgpContext.R1 * wgpContext.T1 * wgpContext.S;

	wgpContext.R2 = Matrix4f::Rotate(-angle2 * _180_ON_PI, 0.0f, 0.0f);
	wgpContext.T2 = Matrix4f::Translate(-focalPoint);
	wgpContext.uniforms.viewMatrix = wgpContext.T2 * wgpContext.R2;

	float ratio = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	float focalLength = 2.0;
	float _near = 0.01f;
	float _far = 100.0f;
	float divider = 1 / (focalLength * (_far - _near));
	wgpContext.uniforms.projectionMatrix = Matrix4f::Transpose(Matrix4f(
		1.0, 0.0, 0.0, 0.0,
		0.0, ratio, 0.0, 0.0,
		0.0, 0.0, _far * divider, -_far * _near * divider,
		0.0, 0.0, 1.0 / focalLength, 0.0
	));

	wgpContext.uniforms.time = 1.0f;
	//wgpContext.uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	wgpContext.uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	wgpuQueueWriteBuffer(wgpContext.queue, wgpContext.uniformBuffer, 0, &wgpContext.uniforms, sizeof(MyUniforms));

	std::vector<WGPUBindGroupEntry> bindings(3);

	bindings[0].binding = 0;
	bindings[0].buffer = wgpContext.uniformBuffer;
	bindings[0].offset = 0;
	bindings[0].size = sizeof(MyUniforms);

	// Create the color texture
	WGPUTextureDescriptor textureDesc = {};
	textureDesc.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
	textureDesc.size = { 512, 512, 1 };
	textureDesc.mipLevelCount = 1;
	textureDesc.sampleCount = 1;
	textureDesc.format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
	textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
	textureDesc.viewFormatCount = 0;
	textureDesc.viewFormats = nullptr;
	wgpContext.texture = wgpuDeviceCreateTexture(wgpContext.device, &textureDesc);
	
	WGPUTextureViewDescriptor textureViewDesc = {};
	textureViewDesc.aspect = WGPUTextureAspect::WGPUTextureAspect_All;
	textureViewDesc.baseArrayLayer = 0;
	textureViewDesc.arrayLayerCount = 1;
	textureViewDesc.baseMipLevel = 0;
	textureViewDesc.mipLevelCount = 1;
	textureViewDesc.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	textureViewDesc.format = textureDesc.format;
	wgpContext.textureView = wgpuTextureCreateView(wgpContext.texture, &textureViewDesc);
	
	bindings[1].binding = 1;
	bindings[1].textureView = wgpContext.textureView;

	WGPUSamplerDescriptor samplerDesc;
	samplerDesc.addressModeU = WGPUAddressMode_ClampToEdge;
	samplerDesc.addressModeV = WGPUAddressMode_ClampToEdge;
	samplerDesc.addressModeW = WGPUAddressMode_ClampToEdge;
	samplerDesc.magFilter = WGPUFilterMode_Linear;
	samplerDesc.minFilter = WGPUFilterMode_Linear;
	samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Linear;
	samplerDesc.lodMinClamp = 0.0f;
	samplerDesc.lodMaxClamp = 1.0f;
	samplerDesc.compare = WGPUCompareFunction_Undefined;
	samplerDesc.maxAnisotropy = 1;
	WGPUSampler sampler = wgpuDeviceCreateSampler(wgpContext.device, &samplerDesc);

	bindings[2].binding = 2;
	bindings[2].sampler = sampler;
	
	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpContext.bindGroupLayout;
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();
	wgpContext.bindGroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	wgpContext.surface_capabilities = { 0 };
	wgpuSurfaceGetCapabilities(wgpContext.surface, wgpContext.adapter, &wgpContext.surface_capabilities);

	wgpContext.config = {};
	wgpContext.config.nextInChain = nullptr;
	wgpContext.config.format = wgpContext.surface_capabilities.formats[0];
	wgpContext.config.width = Application::Width;
	wgpContext.config.height = Application::Height;
	wgpContext.config.usage = WGPUTextureUsage_RenderAttachment;
	wgpContext.config.viewFormatCount = 0;
	wgpContext.config.viewFormats = nullptr;
	wgpContext.config.device = wgpContext.device;
	wgpContext.config.presentMode = WGPUPresentMode_Fifo;
	wgpContext.config.alphaMode = wgpContext.surface_capabilities.alphaModes[0];

	wgpuSurfaceConfigure(wgpContext.surface, &wgpContext.config);

	return true;
}

WGPUPipelineLayout wgpCreatePipelineLayout() {
	const WGPUDevice& device = wgpContext.device;


	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	WGPUBindGroupLayoutEntry& bindingLayout = bindingLayoutEntries[0];
	bindingLayout.binding = 0;
	bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayout.buffer.minBindingSize = sizeof(MyUniforms);

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries[1];
	textureBindingLayout.binding = 1;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType_Float;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension_2D;
	
	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries[2];
	samplerBindingLayout.binding = 2;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bglDesc = {};
	bglDesc.entryCount = (uint32_t)bindingLayoutEntries.size();
	bglDesc.entries = bindingLayoutEntries.data();
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
	textureDescriptor.viewFormats = &textureFormat;
	return wgpuDeviceCreateTexture(device, &textureDescriptor);
}

WGPUTextureView wgpCreateTextureView(WGPUTextureFormat textureFormat, WGPUTextureAspect aspect, const WGPUTexture& texture) {
	WGPUTextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.aspect = aspect;
	textureViewDescriptor.baseArrayLayer = 0;
	textureViewDescriptor.arrayLayerCount = 1;
	textureViewDescriptor.baseMipLevel = 0;
	textureViewDescriptor.mipLevelCount = 1;
	textureViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	textureViewDescriptor.format = textureFormat;
	textureViewDescriptor.nextInChain = nullptr;
	return wgpuTextureCreateView(texture, &textureViewDescriptor);
}

void wgpResize(uint32_t width, uint32_t height) {
	if (wgpContext.surface) {
		wgpuTextureViewRelease(wgpContext.depthTextureView);
		wgpuTextureDestroy(wgpContext.depthTexture);
		wgpuTextureRelease(wgpContext.depthTexture);

		wgpContext.depthTexture = wgpCreateTexture(width, height, WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, WGPUTextureUsage_RenderAttachment);
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
	wgpContext.uniforms.time = static_cast<float>(Globals::clock.getElapsedTimeSec());
	wgpuQueueWriteBuffer(wgpContext.queue, wgpContext.uniformBuffer, offsetof(MyUniforms, time), &wgpContext.uniforms.time, sizeof(MyUniforms::time));
	wgpContext.angle1 = wgpContext.uniforms.time;
	wgpContext.R1 = Matrix4f::Rotate(0.0f, 0.0f, wgpContext.angle1 * _180_ON_PI);
	wgpContext.uniforms.modelMatrix = wgpContext.R1 * wgpContext.T1 * wgpContext.S;
	wgpuQueueWriteBuffer(wgpContext.queue, wgpContext.uniformBuffer, offsetof(MyUniforms, modelMatrix), &wgpContext.uniforms.modelMatrix, sizeof(MyUniforms::modelMatrix));

	WGPUSurfaceTexture surface_texture;
	wgpuSurfaceGetCurrentTexture(wgpContext.surface, &surface_texture);

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
	//assert(surface_texture.texture);

	WGPUTextureView frame = wgpuTextureCreateView(surface_texture.texture, NULL);

	WGPUCommandEncoderDescriptor commandEncoderDesc = {};
	commandEncoderDesc.label = { "command_encoder", WGPU_STRLEN };
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDesc);

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

	WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
	depthStencilAttachment.view = wgpContext.depthTextureView;
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

	wgpuRenderPassEncoderSetPipeline(renderPass, wgpContext.pipeline);
	wgpuRenderPassEncoderSetBindGroup(renderPass, 0, wgpContext.bindGroup, 0, nullptr);

	wgpContext.OnDraw(renderPass);

	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);
	wgpuTextureViewRelease(frame);

	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	cmdBufferDescriptor.label = { "Command buffer", WGPU_STRLEN };
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
	wgpuQueueSubmit(wgpContext.queue, 1, &command);

	wgpuSurfacePresent(wgpContext.surface);

	wgpuCommandBufferRelease(command);
	wgpuCommandEncoderRelease(encoder);
	
	wgpuTextureRelease(surface_texture.texture);
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