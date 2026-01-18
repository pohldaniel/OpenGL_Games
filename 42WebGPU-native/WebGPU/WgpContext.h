#pragma once

#include <array>
#include <vector>
#include <string>
#include <functional>
#include <webgpu.h>
#include <engine/Vector.h>

struct WgpContext;
enum VertexLayoutSlot {
	VL_P,
	VL_PT,
	VL_PN,
	VL_PTN,
};

extern WgpContext wgpContext;
extern std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexAttribute>> wgpVertexAttributes;
extern std::unordered_map<VertexLayoutSlot, WGPUVertexBufferLayout> wgpVertexBufferLayouts;

extern "C" {
	void wgpInit(void* window);
	bool wgpCreateDevice(WgpContext& wgpContext, void* window);

	WGPUBuffer wgpCreateBuffer(uint32_t size, WGPUBufferUsage bufferUsage);

	WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, WGPUTextureFormat viewFormat = WGPUTextureFormat_Undefined);
	WGPUTextureView wgpCreateTextureView(WGPUTextureFormat textureFormat, WGPUTextureAspect aspect, const WGPUTexture& texture);
	WGPUSampler wgpCreateSampler();
	WGPUShaderModule wgpCreateShader(std::string path);
	void wgpCreateVertexBufferLayout(VertexLayoutSlot slot = VL_PTN);
	void wgpShutDown();
	void wgpPipelinesRelease();
	void wgpSamplersRelease();

	void wgpDraw();
	void wgpResize(uint32_t width, uint32_t height);
	void wgpToggleVerticalSync();
}

enum RenderPipelineSlot {
	RP_PTN,
	RP_WIREFRAME
};

struct WgpContext {

	friend void wgpPipelinesRelease();
	friend void wgpSamplersRelease();

	WGPURenderPipeline createRenderPipelinePTN(std::string path);
	void createVertexBufferLayout(VertexLayoutSlot slot = VL_PTN);
	void addSampler(const WGPUSampler& sampler);
	const std::vector<WGPUSampler>& getSamplers();

	WGPUInstance instance = nullptr;
	WGPUSurface surface = nullptr;

	WGPUAdapter adapter = nullptr;
	WGPUDevice device = nullptr;
	WGPUQueue queue = nullptr;
	WGPUTextureView depthTextureView = nullptr;
	WGPUTexture depthTexture = nullptr;
	WGPUSurfaceConfiguration config = {};
	WGPUSurfaceCapabilities surfaceCapabilities;

	std::unordered_map<RenderPipelineSlot, WGPURenderPipeline> renderPipelines;
	WGPUTextureFormat depthformat = WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;
		
	std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw;
	std::function <WGPUBindGroupLayout()> OnBindGroupLayout;
	std::function <void(const WGPUBindGroupLayout&)> OnBindGroup;
	std::function<WGPUPipelineLayout(const WGPUBindGroupLayout&)> OnPipelineLayout;	



private:
	std::vector<WGPUPipelineLayout> pipelineLayouts;
	std::vector<WGPUShaderModule> shaderModules;
	std::vector<WGPUSampler> samplers;
};