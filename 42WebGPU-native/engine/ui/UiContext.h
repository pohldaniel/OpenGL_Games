#pragma once

#include <vector>
#include <webgpu.h>
#include <WebGPU/WgpBuffer.h>

struct UiContext;
extern UiContext uiContext;

extern "C" {
	void uiInit(float width, float height);
	void uiResize(float width, float height);
	void uiShutDown();
	void uiDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);

	void uiCreateRenderPipeline(WGPURenderPipeline& renderPipeline);
	void uiCreateRenderPipelineMask(WGPURenderPipeline& renderPipeline);
	void uiCreateRenderPipelineRead(WGPURenderPipeline& renderPipeline);
	void uiCreateRenderPipelineText(WGPURenderPipeline& renderPipeline);
	void uiCreateBindGroup(WGPUBindGroup& bindgroup);
	void uiCreateBindGroupText(WGPUBindGroup& bindgroup);
}
	
struct UiInstance {
	float transform[16];
	float color[4];
	float textureRect[4];
	float textureLayer;
	float flipAndTile[3];
};

enum class UiPipelineType {
	Standard,
	MaskWrite,
	OutlineRead,
	Text
};

struct UiBatch {
	UiPipelineType pipelineType;
	uint32_t startIndex;
	uint32_t instanceCount;
};

struct UiContext {
	float width;
	float height;

	WGPUBindGroup bindgroup = nullptr, bindgroupText = nullptr;
	WGPUBindGroupLayout bindgroupLayout = nullptr, bindgroupLayoutText = nullptr;
	WGPURenderPipeline renderPipeline = nullptr, renderPipelineMask = nullptr, renderPipelineRead = nullptr, renderPipelineText = nullptr;
	WGPUTextureView textureView = nullptr;

	WgpBuffer wgpStorageBuffer, wgpUniformBuffer;

	std::vector<UiInstance> uiInstances;
	std::vector<UiBatch> uiBatches;
};