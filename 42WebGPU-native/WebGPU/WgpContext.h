#pragma once

#include <array>
#include <vector>
#include <string>
#include <functional>
#include <webgpu.h>
#include <wgpu.h>
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
	void wgpShaderModulesRelease();
	void wgpPipelineLayoutsRelease();

	void wgpDraw();
	void wgpResize(uint32_t width, uint32_t height);
	void wgpToggleVerticalSync();
}

enum RenderPipelineSlot {
	RP_PTN,
	RP_WIREFRAME
};

enum SamplerSlot {
	SS_LINEAR
};

struct WgpContext {

	friend void wgpPipelinesRelease();
	friend void wgpSamplersRelease();
	friend void wgpShaderModulesRelease();
	friend void wgpPipelineLayoutsRelease();

	WGPURenderPipeline createRenderPipelinePTN(std::string shaderModuleName);
	WGPURenderPipeline createRenderPipelineWireframe(std::string shaderModuleName);

	void createVertexBufferLayout(VertexLayoutSlot slot = VL_PTN);
	void addSampler(const WGPUSampler& sampler, SamplerSlot samplerSlot = SS_LINEAR);
	const WGPUSampler& getSampler(SamplerSlot samplerSlot);
	void addSahderModule(const std::string& shaderModuleName, const std::string& shaderModulePath);
	const WGPUShaderModule& getShaderModule(std::string shaderModuleName);

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

private:
	std::unordered_map<RenderPipelineSlot, WGPUPipelineLayout> pipelineLayouts;
	std::unordered_map<SamplerSlot, WGPUSampler> samplers;
	std::unordered_map<std::string, WGPUShaderModule> shaderModules;
};