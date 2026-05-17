#include "WgpTexture.h"
#include "WgpBuffer.h"
#include "WgpContext.h"
#include "WgpRenderer.h"

void WgpRenderer::Draw(const WgpTexture& texture, std::function<void(const WGPURenderPassEncoder& commandBuffer, uint32_t layer, uint32_t mip)> OnDraw) {
	uint32_t arrayLayerCount = wgpuTextureGetDepthOrArrayLayers(texture.getTexture());
	uint32_t mipLevelCount = wgpuTextureGetMipLevelCount(texture.getTexture());
	WGPUTextureFormat textureFormat = wgpuTextureGetFormat(texture.getTexture());

	float mipWidth = static_cast<float>(wgpuTextureGetWidth(texture.getTexture()));
	float mipHeight = static_cast<float>(wgpuTextureGetHeight(texture.getTexture()));

	for (uint32_t mip = 0u; mip < mipLevelCount; ++mip) {

		for (uint32_t layer = 0u; layer < arrayLayerCount; ++layer) {
			WGPUTextureViewDescriptor textureViewDescriptor = {};
			textureViewDescriptor.label = WGPU_STR("texture_view");
			textureViewDescriptor.aspect = WGPUTextureAspect_All;
			textureViewDescriptor.baseArrayLayer = layer;
			textureViewDescriptor.arrayLayerCount = 1u;
			textureViewDescriptor.baseMipLevel = mip;
			textureViewDescriptor.mipLevelCount = 1u;
			textureViewDescriptor.dimension = WGPUTextureViewDimension_2D;
			textureViewDescriptor.format = textureFormat;
			textureViewDescriptor.nextInChain = NULL;

			WGPUTextureView textureView = wgpuTextureCreateView(texture.getTexture(), &textureViewDescriptor);

			WGPURenderPassColorAttachment renderPassColorAttachment = {};
			renderPassColorAttachment.view = textureView;
			renderPassColorAttachment.resolveTarget = NULL;
			renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
			renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
			renderPassColorAttachment.clearValue = { 0.0f, 1.0f, 0.0f, 1.0f };
			renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

			WGPURenderPassDescriptor renderPassDesc = {};
			renderPassDesc.colorAttachmentCount = 1;
			renderPassDesc.colorAttachments = &renderPassColorAttachment;
			renderPassDesc.timestampWrites = NULL;

			WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
			commandEncoderDescriptor.label = WGPU_STR("command_encoder");

			WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDescriptor);
			WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);
			wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, mipWidth, mipHeight, 0.0f, 1.0f);

			OnDraw(renderPassEncoder, layer, mip);

			wgpuRenderPassEncoderEnd(renderPassEncoder);
			wgpuRenderPassEncoderRelease(renderPassEncoder);
			wgpuTextureViewRelease(textureView);

			WGPUCommandBufferDescriptor commandBufferDescriptor = {};
			commandBufferDescriptor.label = WGPU_STR("command_buffer");
			WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
			wgpuCommandEncoderRelease(commandEncoder);

			wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);
			wgpuCommandBufferRelease(commandBuffer);
		}
		mipWidth *= 0.5f;
		mipHeight *= 0.5f;
	}
}

void WgpRenderer::DrawDepth(const WgpTexture& texture, std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw) {
	float mipWidth = static_cast<float>(wgpuTextureGetWidth(texture.getTexture()));
	float mipHeight = static_cast<float>(wgpuTextureGetHeight(texture.getTexture()));

	WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
	depthStencilAttachment.view = texture.getTextureView();
	depthStencilAttachment.depthClearValue = 1.0f;
	depthStencilAttachment.depthLoadOp = WGPULoadOp::WGPULoadOp_Clear;
	depthStencilAttachment.depthStoreOp = WGPUStoreOp::WGPUStoreOp_Store;
	depthStencilAttachment.depthReadOnly = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilAttachment.stencilClearValue = 0u;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp::WGPULoadOp_Undefined;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp::WGPUStoreOp_Undefined;
	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool::WGPUOptionalBool_True;

	WGPURenderPassDescriptor renderPassDesc = {};
	renderPassDesc.label = WGPU_STR("RenderPass");
	renderPassDesc.colorAttachmentCount = 0u;
	renderPassDesc.colorAttachments = NULL;
	renderPassDesc.timestampWrites = NULL;
	renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

	WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
	commandEncoderDescriptor.label = WGPU_STR("command_encoder");

	WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDescriptor);
	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, mipWidth, mipHeight, 0.0f, 1.0f);

	OnDraw(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);

	WGPUCommandBufferDescriptor commandBufferDescriptor = {};
	commandBufferDescriptor.label = WGPU_STR("command_buffer");
	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
	wgpuCommandEncoderRelease(commandEncoder);

	wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);
	wgpuCommandBufferRelease(commandBuffer);
}

void WgpRenderer::Dispatch(const WgpTexture& texture, const WgpBuffer& probability, const WgpBuffer& bufferA, const WgpBuffer& bufferB) {
	uint32_t mipLevelCount = wgpuTextureGetMipLevelCount(texture.getTexture());
	uint32_t mipWidth = wgpuTextureGetWidth(texture.getTexture());
	uint32_t mipHeight = wgpuTextureGetHeight(texture.getTexture());
	uint32_t depth = wgpuTextureGetDepthOrArrayLayers(texture.getTexture());
	WGPUTextureFormat textureFormat = wgpuTextureGetFormat(texture.getTexture());

	WgpTexture writeTexture;
	writeTexture.createEmpty(mipWidth, mipHeight, depth,WGPUTextureUsage_StorageBinding | WGPUTextureUsage_CopySrc, textureFormat, mipLevelCount);
	writeTexture.markForDelete();

	std::vector<WGPUTextureView> textureViewsRead = std::vector<WGPUTextureView>(mipLevelCount);
	std::vector<WGPUTextureView> textureViewsWrite = std::vector<WGPUTextureView>(mipLevelCount);
	std::vector<WGPUBindGroup> bindGroups = std::vector<WGPUBindGroup>(mipLevelCount);

	WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, NULL);
	for (uint32_t level = 0; level < mipLevelCount; ++level) {
		const uint32_t levelWidth = mipWidth >> level;
		const uint32_t levelHeight = mipHeight >> level;

		const WGPUBindGroupLayout pipelineLayout
			= level == 0 ? wgpuComputePipelineGetBindGroupLayout(wgpContext.computePipelines.at("CP_IMPORT"), 0) :
			               wgpuComputePipelineGetBindGroupLayout(wgpContext.computePipelines.at("CP_EXPORT"), 0);

		WGPUTextureViewDescriptor textureViewDescriptor = {};
		textureViewDescriptor.label = WGPU_STR("texture_view");
		textureViewDescriptor.aspect = WGPUTextureAspect_All;
		textureViewDescriptor.baseArrayLayer = 0u;
		textureViewDescriptor.arrayLayerCount = 1u;
		textureViewDescriptor.baseMipLevel = level;
		textureViewDescriptor.mipLevelCount = 1u;
		textureViewDescriptor.dimension = WGPUTextureViewDimension_2D;
		textureViewDescriptor.format = textureFormat;
		textureViewDescriptor.nextInChain = NULL;

		textureViewsRead[level] = wgpuTextureCreateView(texture.getTexture(), &textureViewDescriptor);
		textureViewsWrite[level] = wgpuTextureCreateView(writeTexture.getTexture(), &textureViewDescriptor);

		std::vector<WGPUBindGroupEntry> bindGroupEntries(5);

		bindGroupEntries[0].binding = 0u;
		bindGroupEntries[0].buffer = probability.getBuffer();
		bindGroupEntries[0].offset = 0u;
		bindGroupEntries[0].size = wgpuBufferGetSize(probability.getBuffer());

		bindGroupEntries[1].binding = 1u;
		bindGroupEntries[1].buffer = level & 1 ? bufferA.getBuffer() : bufferB.getBuffer();
		bindGroupEntries[1].offset = 0u;
		bindGroupEntries[1].size = wgpuBufferGetSize(bindGroupEntries[1].buffer);

		bindGroupEntries[2].binding = 2u;
		bindGroupEntries[2].buffer = level & 1 ? bufferB.getBuffer() : bufferA.getBuffer();
		bindGroupEntries[2].offset = 0u;
		bindGroupEntries[2].size = wgpuBufferGetSize(bindGroupEntries[1].buffer);

		bindGroupEntries[3].binding = 3u;
		bindGroupEntries[3].textureView = textureViewsRead[level];

		bindGroupEntries[4].binding = 4u;
		bindGroupEntries[4].textureView = textureViewsWrite[level];

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = pipelineLayout;
		bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
		bindGroupDesc.entries = bindGroupEntries.data();

		bindGroups[level] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

		if (level == 0) {
			WGPUComputePassEncoder computePassEncoder = wgpuCommandEncoderBeginComputePass(commandEncoder, NULL);
			wgpuComputePassEncoderSetPipeline(computePassEncoder, wgpContext.computePipelines.at("CP_IMPORT"));
			wgpuComputePassEncoderSetBindGroup(computePassEncoder, 0u, bindGroups[level], 0u, NULL);
			wgpuComputePassEncoderDispatchWorkgroups(computePassEncoder, ceil(levelWidth / 64.f), levelHeight, 1u);
			wgpuComputePassEncoderEnd(computePassEncoder);
			wgpuComputePassEncoderRelease(computePassEncoder);
		}else {
			WGPUComputePassEncoder computePassEncoder = wgpuCommandEncoderBeginComputePass(commandEncoder, NULL);
			wgpuComputePassEncoderSetPipeline(computePassEncoder, wgpContext.computePipelines.at("CP_EXPORT"));
			wgpuComputePassEncoderSetBindGroup(computePassEncoder, 0u, bindGroups[level], 0u, NULL);
			wgpuComputePassEncoderDispatchWorkgroups(computePassEncoder, ceil(levelWidth / 64.f), levelHeight, 1u);
			wgpuComputePassEncoderEnd(computePassEncoder);
			wgpuComputePassEncoderRelease(computePassEncoder);

			WGPUTexelCopyTextureInfo source = {};
			source.texture = texture.getTexture();
			source.mipLevel = level;
			source.origin = { 0u, 0u,  0u };
			source.aspect = WGPUTextureAspect_All;

			WGPUTexelCopyTextureInfo destination = {};
			destination.texture = writeTexture.getTexture();
			destination.mipLevel = level;
			destination.origin = { 0u, 0u, 0u };
			destination.aspect = WGPUTextureAspect_All;

			WGPUExtent3D size = { levelWidth , levelHeight, 1u };

			wgpuCommandEncoderCopyTextureToTexture(commandEncoder, &destination, &source, &size);
		}	
	}

	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, NULL);
	wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);

	wgpuCommandEncoderRelease(commandEncoder);
	wgpuCommandBufferRelease(commandBuffer);

	for (uint32_t i = 0; i < mipLevelCount; ++i) {
		wgpuTextureViewRelease(textureViewsRead[i]);
	}

	for (uint32_t i = 0; i < mipLevelCount; ++i) {
		wgpuTextureViewRelease(textureViewsWrite[i]);
	}

	for (uint32_t i = 0; i < mipLevelCount; ++i) {
		wgpuBindGroupRelease(bindGroups[i]);
	}
}