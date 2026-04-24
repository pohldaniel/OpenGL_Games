#include "WgpTexture.h"
#include "WgpContext.h"
#include "WgpRenderer.h"

#include <iostream>
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