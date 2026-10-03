#include <WebGPU/WgpContext.h>
#include "UiContext.h"

UiContext uiContext = {};

void uiInit(float width, float height) {
	uiContext.width = width;
	uiContext.height = height;

	uiContext.activeScissorWidth = static_cast<uint32_t>(width);
	uiContext.activeScissorHeight = static_cast<uint32_t>(height);

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

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntriesText(4);

	bindingLayoutEntriesText[0].binding = 0u;
	bindingLayoutEntriesText[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntriesText[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntriesText[0].buffer.minBindingSize = 16u * sizeof(float);

	bindingLayoutEntriesText[1].binding = 1u;
	bindingLayoutEntriesText[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntriesText[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntriesText[1].buffer.minBindingSize = 400u * sizeof(UiInstance);

	bindingLayoutEntriesText[2].binding = 2u;
	bindingLayoutEntriesText[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntriesText[2].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntriesText[3].binding = 3u;
	bindingLayoutEntriesText[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntriesText[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntriesText[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2DArray;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptorText = {};
	bindGroupLayoutDescriptorText.entryCount = (uint32_t)bindingLayoutEntriesText.size();
	bindGroupLayoutDescriptorText.entries = bindingLayoutEntriesText.data();

	uiContext.bindgroupLayoutText = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptorText);

	uiCreateRenderPipeline(uiContext.renderPipeline);
	uiCreateRenderPipelineMask(uiContext.renderPipelineMask);
	uiCreateRenderPipelineRead(uiContext.renderPipelineRead);
	uiCreateRenderPipelineClear(uiContext.renderPipelineClear);
	uiCreateRenderPipelineText(uiContext.renderPipelineText);

	uiCreateBindGroup(uiContext.bindgroup);
	uiCreateBindGroupText(uiContext.bindgroupText);
}

void uiResize(float width, float height) {
	uiContext.width = width;
	uiContext.height = height;

	uiContext.activeScissorWidth = static_cast<uint32_t>(width);
	uiContext.activeScissorHeight = static_cast<uint32_t>(height);

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

	if (uiContext.bindgroupText) {
		wgpuBindGroupRelease(uiContext.bindgroupText);
		uiContext.bindgroup = NULL;
	}

	if (uiContext.bindgroupLayout) {
		wgpuBindGroupLayoutRelease(uiContext.bindgroupLayout);
		uiContext.bindgroupLayout = NULL;
	}

	if (uiContext.bindgroupLayoutText) {
		wgpuBindGroupLayoutRelease(uiContext.bindgroupLayoutText);
		uiContext.bindgroupLayoutText = NULL;
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

	if (uiContext.renderPipelineClear) {
		wgpuRenderPipelineRelease(uiContext.renderPipelineClear);
		uiContext.renderPipelineClear = NULL;
	}

	if (uiContext.renderPipelineText) {
		wgpuRenderPipelineRelease(uiContext.renderPipelineText);
		uiContext.renderPipelineText = NULL;
	}

	
	for (auto& layer : uiContext.uiLayers) {
		layer.batches.clear();
		layer.batches.shrink_to_fit();
	}

	uiContext.uiLayers.clear();
	uiContext.uiLayers.shrink_to_fit();

	uiContext.uiInstances.clear();
	uiContext.uiInstances.shrink_to_fit();
}

void uiDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	wgpuQueueWriteBuffer(wgpContext.queue, uiContext.wgpStorageBuffer.getBuffer(), 0u, uiContext.uiInstances.data(), uiContext.uiInstances.size() * sizeof(UiInstance));

	WGPURenderPassDepthStencilAttachment depthStencilAttachment = wgpCopyDepthStencilAttachment(renderPassDescriptor.depthStencilAttachment);
	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
	depthStencilAttachment.stencilClearValue = 0u;

	WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
	rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);

	for (const auto& layer : uiContext.uiLayers) {
		wgpuRenderPassEncoderSetStencilReference(renderPassEncoder, 1u);

		const auto& batch = layer.batches.front();
		if (batch.pipelineType == UiPipelineType::Clear) {
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, uiContext.bindgroup, 0u, nullptr);
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipelineClear);
			wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, batch.instanceCount, 0u, batch.startIndex);
		}

		for (const auto& batch : layer.batches) {
			if (batch.instanceCount == 0) continue;

			wgpuRenderPassEncoderSetScissorRect(renderPassEncoder, batch.scissorX, batch.scissorY, batch.scissorWidth, batch.scissorHeight);

			switch (batch.pipelineType) {
			case UiPipelineType::Standard: case UiPipelineType::Clear:
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, uiContext.bindgroup, 0u, nullptr);
				wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipeline);
				break;
			case UiPipelineType::MaskWrite:
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, uiContext.bindgroup, 0u, nullptr);
				wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipelineMask);
				break;
			case UiPipelineType::OutlineRead:
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, uiContext.bindgroup, 0u, nullptr);
				wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipelineRead);
				break;
			case UiPipelineType::Text:
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, uiContext.bindgroupText, 0u, nullptr);
				wgpuRenderPassEncoderSetPipeline(renderPassEncoder, uiContext.renderPipelineText);
				break;
			}
			wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, batch.instanceCount, 0u, batch.startIndex);
		}	
	}

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
	uiContext.uiInstances.clear();
	uiContext.uiLayers.clear();
	uiContext.currentActiveLayer = nullptr;
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

void uiCreateBindGroupText(WGPUBindGroup& bindgroup) {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(4);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = uiContext.wgpUniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(uiContext.wgpUniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = uiContext.wgpStorageBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(uiContext.wgpStorageBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = uiContext.textureView;

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = uiContext.bindgroupLayoutText;
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

void uiCreateRenderPipelineClear(WGPURenderPipeline& renderPipeline) {
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
	colorTargetState.writeMask = WGPUColorWriteMask_None;

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
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Zero;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Zero;

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

void uiCreateRenderPipelineText(WGPURenderPipeline& renderPipeline) {
	WGPUShaderModule shaderModule = wgpCreateShaderFromFile("res/shader/ui_text.wgsl");

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &uiContext.bindgroupLayoutText;
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