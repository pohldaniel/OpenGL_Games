#include <WebGPU/WgpContext.h>
#include "UiContext.h"

UiContext uiContext = {};

void uiInit(float width, float height) {
	uiContext.width = width;
	uiContext.height = height;

	uiContext.wgpStorageBuffer.createBuffer(400u * sizeof(UiInstance), WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);
	uiContext.wgpUniformBuffer.createBuffer(16 * sizeof(float), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	uiResize(width, height);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 16u * sizeof(float);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 400u * sizeof(UiInstance);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	uiContext.bindgroupLayout = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	uiCreateRenderPipeline(uiContext.renderPipeline);
	uiCreateRenderPipelineMask(uiContext.renderPipelineMask);
	uiCreateRenderPipelineRead(uiContext.renderPipelineRead);

	uiCreateBindGroup(uiContext.bindgroup);
}

void uiResize(float width, float height) {
	uiContext.width = width;
	uiContext.height = height;

	float ortho[16] = { 2.0f / width, 0.0f,   0.0f, 0.0f,
						0.0f, -2.0f / height, 0.0f, 0.0f,
						0.0f, 0.0f, -1.0f, 0.0f,
					   -1.0f, 1.0f,  0.0f, 1.0f };

	wgpuQueueWriteBuffer(wgpContext.queue, uiContext.wgpUniformBuffer.getBuffer(), 0u, ortho, 16 * sizeof(float));
}

void uiShutDown() {
	uiContext.wgpUniformBuffer.markForDelete();
	uiContext.wgpUniformBuffer.cleanup();

	uiContext.wgpStorageBuffer.markForDelete();
	uiContext.wgpStorageBuffer.cleanup();

	if (uiContext.bindgroup) {
		wgpuBindGroupRelease(uiContext.bindgroup);
		uiContext.bindgroup = NULL;
	}

	if (uiContext.bindgroupLayout) {
		wgpuBindGroupLayoutRelease(uiContext.bindgroupLayout);
		uiContext.bindgroupLayout = NULL;
	}

	if (uiContext.renderPipeline) {
		wgpuRenderPipelineRelease(uiContext.renderPipeline);
		uiContext.renderPipeline = NULL;
	}

	if (uiContext.renderPipelineMask) {
		wgpuRenderPipelineRelease(uiContext.renderPipelineMask);
		uiContext.renderPipelineMask = NULL;
	}

	if (uiContext.renderPipelineRead) {
		wgpuRenderPipelineRelease(uiContext.renderPipelineRead);
		uiContext.renderPipelineRead = NULL;
	}

	uiContext.uiBatches.clear();
	uiContext.uiBatches.shrink_to_fit();

	uiContext.uiInstances.clear();
	uiContext.uiInstances.shrink_to_fit();
}

void uiDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	wgpuQueueWriteBuffer(wgpContext.queue, uiContext.wgpStorageBuffer.getBuffer(), 0u, uiContext.uiInstances.data(), uiContext.uiInstances.size() * sizeof(UiInstance));

	WGPURenderPassDepthStencilAttachment depthStencilAttachment = wgpCopyDepthStencilAttachment(renderPassDescriptor.depthStencilAttachment);
	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
	depthStencilAttachment.stencilClearValue = 0;

	WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
	rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, uiContext.bindgroup, 0u, nullptr);

	wgpuRenderPassEncoderSetStencilReference(renderPassEncoder, 1);

	for (const auto& batch : uiContext.uiBatches) {
		if (batch.instanceCount == 0) continue;

		switch (batch.pipelineType) {
		case UiPipelineType::Standard:
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipeline);
			break;
		case UiPipelineType::MaskWrite:
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipelineMask);
			break;
		case UiPipelineType::OutlineRead:
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipelineRead);
			break;
		}

		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, batch.instanceCount, 0u, batch.startIndex);
	}

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);

	uiContext.uiInstances.clear();
	uiContext.uiBatches.clear();
}

void uiCreateBindGroup(WGPUBindGroup& bindgroup) {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = uiContext.wgpUniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(uiContext.wgpUniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = uiContext.wgpStorageBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(uiContext.wgpStorageBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = uiContext.bindgroupLayout;
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindgroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

void uiCreateRenderPipeline(WGPURenderPipeline& renderPipeline) {
	WGPUShaderModule shaderModule = wgpCreateShaderFromFile("res/shader/ui.wgsl");

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &uiContext.bindgroupLayout;
	WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 0u;
	vertexState.buffers = nullptr;

	WGPUBlendState blendState = {};
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;

	WGPUColorTargetState colorTargetState = {};
	colorTargetState.nextInChain = NULL;
	colorTargetState.format = wgpContext.colorFormat;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0u;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1u;
	fragmentState.targets = &colorTargetState;

	WGPUDepthStencilState depthStencilState = {};
	depthStencilState.nextInChain = NULL;
	depthStencilState.format = wgpContext.depthFormat;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

	depthStencilState.stencilFront.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilFront.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;

	depthStencilState.stencilReadMask = 0u;
	depthStencilState.stencilWriteMask = 0u;

	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayout;
	renderPipelineDescriptor.multisample.count = wgpContext.msaaSampleCount;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

	renderPipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);

	wgpuShaderModuleRelease(shaderModule);
	wgpuPipelineLayoutRelease(pipelineLayout);
}

void uiCreateRenderPipelineMask(WGPURenderPipeline& renderPipeline) {
	WGPUShaderModule shaderModule = wgpCreateShaderFromFile("res/shader/ui.wgsl");

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &uiContext.bindgroupLayout;
	WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 0u;
	vertexState.buffers = nullptr;

	WGPUBlendState blendState = {};
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;

	WGPUColorTargetState colorTargetState = {};
	colorTargetState.nextInChain = NULL;
	colorTargetState.format = wgpContext.colorFormat;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0u;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1u;
	fragmentState.targets = &colorTargetState;

	WGPUDepthStencilState depthStencilState = {};
	depthStencilState.nextInChain = NULL;
	depthStencilState.format = wgpContext.depthFormat;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

	depthStencilState.stencilFront.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilFront.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;

	depthStencilState.stencilReadMask = 0xFFFFFFFF;
	depthStencilState.stencilWriteMask = 0xFFFFFFFF;

	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayout;
	renderPipelineDescriptor.multisample.count = wgpContext.msaaSampleCount;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

	renderPipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);

	wgpuShaderModuleRelease(shaderModule);
	wgpuPipelineLayoutRelease(pipelineLayout);
}

void uiCreateRenderPipelineRead(WGPURenderPipeline& renderPipeline) {
	WGPUShaderModule shaderModule = wgpCreateShaderFromFile("res/shader/ui.wgsl");

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &uiContext.bindgroupLayout;
	WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 0u;
	vertexState.buffers = nullptr;

	WGPUBlendState blendState = {};
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;

	WGPUColorTargetState colorTargetState = {};
	colorTargetState.nextInChain = NULL;
	colorTargetState.format = wgpContext.colorFormat;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0u;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1u;
	fragmentState.targets = &colorTargetState;

	WGPUDepthStencilState depthStencilState = {};
	depthStencilState.nextInChain = NULL;
	depthStencilState.format = wgpContext.depthFormat;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

	depthStencilState.stencilFront.compare = WGPUCompareFunction::WGPUCompareFunction_NotEqual;
	depthStencilState.stencilFront.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_NotEqual;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;

	depthStencilState.stencilReadMask = 0xFFFFFFFF;
	depthStencilState.stencilWriteMask = 0xFFFFFFFF;
	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayout;
	renderPipelineDescriptor.multisample.count = wgpContext.msaaSampleCount;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

	renderPipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);

	wgpuShaderModuleRelease(shaderModule);
	wgpuPipelineLayoutRelease(pipelineLayout);
}