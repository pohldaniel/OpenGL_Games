#pragma once

#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <webgpu.h>
#ifdef WEBGPU_NATIVE
	#include <wgpu.h>
#endif

#define WGPU_STR(str) { str, sizeof(str) - 1 }

enum VertexLayoutSlot {
	VL_NONE,
	VL_P,
	VL_PT,
	VL_PN,
	VL_PTN,
	VL_PTNC
};

struct WgpContext;
extern WgpContext wgpContext;
extern std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexAttribute>> wgpVertexAttributes;
extern std::unordered_map<VertexLayoutSlot, WGPUVertexBufferLayout> wgpVertexBufferLayouts;

extern "C" {
	void wgpInit(void* window);
	bool wgpCreateDevice(WgpContext& wgpContext, void* window);

	WGPUBuffer wgpCreateEmptyBuffer(uint32_t size, WGPUBufferUsage bufferUsage);
	WGPUBuffer wgpCreateBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage);
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
	void wgpConfigureSurface();
}

enum SamplerSlot {
	SS_LINEAR
};

struct WgpContext {

	friend void wgpPipelinesRelease();
	friend void wgpSamplersRelease();
	friend void wgpShaderModulesRelease();
	friend void wgpPipelineLayoutsRelease();

	void createComputePipeline(std::string shaderModuleName, std::string pipelineLayoutName, const std::function<WGPUBindGroupLayout()>& onBindGroupLayout);
	void createRenderPipeline(std::string shaderModuleName, std::string pipelineLayoutName, const VertexLayoutSlot vertexLayoutSlot, const std::function<WGPUBindGroupLayout()>& onBindGroupLayout, WGPUPrimitiveTopology primitiveTopology = WGPUPrimitiveTopology::WGPUPrimitiveTopology_TriangleList);

	WGPUBindGroup OnBindGroupPTN(const WGPUBuffer& buffer, const WGPUTextureView& textureView);
	WGPUBindGroup OnBindGroupWF(const WGPUBuffer& uniformBuffer, const WGPUBuffer& vertexBuffer, const WGPUBuffer& indexBuffer);

	void createVertexBufferLayout(VertexLayoutSlot slot = VL_PTN);
	void addSampler(const WGPUSampler& sampler, SamplerSlot samplerSlot = SS_LINEAR);
	const WGPUSampler& getSampler(SamplerSlot samplerSlot);
	void addSahderModule(const std::string& shaderModuleName, const std::string& shaderModulePath);
	const WGPUShaderModule& getShaderModule(std::string shaderModuleName);

	WGPUInstance instance = NULL;
	WGPUAdapter adapter = NULL;
	WGPUDevice device = NULL;
	WGPUSurface surface = NULL;
	WGPUQueue queue = NULL;

	WGPUTextureView depthTextureView = NULL;
	WGPUTexture depthTexture = NULL;
	WGPUSurfaceConfiguration config = {};
	WGPUSurfaceCapabilities surfaceCapabilities;
	WGPUTextureFormat depthformat = WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;
	WGPUTextureFormat colorformat = WGPUTextureFormat::WGPUTextureFormat_BGRA8UnormSrgb;

	std::unordered_map<std::string, WGPUComputePipeline> computePipelines;
	std::unordered_map<std::string, WGPURenderPipeline> renderPipelines;
	std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw = NULL;

private:

	std::unordered_map<std::string, WGPUPipelineLayout> pipelineLayouts;
	std::unordered_map<SamplerSlot, WGPUSampler> samplers;
	std::unordered_map<std::string, WGPUShaderModule> shaderModules;
};