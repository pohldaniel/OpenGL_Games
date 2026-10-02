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
	void uiCreateRenderPipelineClear(WGPURenderPipeline& renderPipeline);
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
	Clear,
	MaskWrite,
	OutlineRead,
	Text
};

struct UiBatch {
	UiPipelineType pipelineType;
	uint32_t startIndex;
	uint32_t instanceCount;
	uint32_t scissorX = 0;
	uint32_t scissorY = 0;
	uint32_t scissorWidth = 0;
	uint32_t scissorHeight = 0;
};

struct UiLayer {
	std::vector<UiBatch> batches;
};

struct UiContext {
	float width;
	float height;

	WGPUBindGroup bindgroup = nullptr, bindgroupText = nullptr;
	WGPUBindGroupLayout bindgroupLayout = nullptr, bindgroupLayoutText = nullptr;
	WGPURenderPipeline renderPipeline = nullptr, renderPipelineMask = nullptr, renderPipelineRead = nullptr, renderPipelineClear = nullptr, renderPipelineText = nullptr;
	WGPUTextureView textureView = nullptr;

	WgpBuffer wgpStorageBuffer, wgpUniformBuffer;

	std::vector<UiInstance> uiInstances;
	std::vector<UiLayer> uiLayers;
	UiLayer* currentActiveLayer = nullptr;

	uint32_t activeScissorX = 0u;
	uint32_t activeScissorY = 0u;
	uint32_t activeScissorWidth = 0u;
	uint32_t activeScissorHeight = 0u;
};