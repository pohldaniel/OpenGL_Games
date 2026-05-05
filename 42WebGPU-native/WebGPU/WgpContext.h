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
	VL_PTNC,
	VL_PTNTB,
	VL_BATCH
};

struct WgpContext;
extern WgpContext wgpContext;
extern std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexAttribute>> wgpVertexAttributes;
extern std::unordered_map<VertexLayoutSlot, WGPUVertexBufferLayout> wgpVertexBufferLayouts;

extern "C" {
	void wgpInit(void* window);
	bool wgpCreateDevice(void* window);

	WGPUBuffer wgpCreateEmptyBuffer(uint32_t size, WGPUBufferUsage bufferUsage);
	WGPUBuffer wgpCreateBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage);
	WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, uint32_t depth, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, uint32_t mipLevelCount = 1u, uint32_t sampleCount = 1u, WGPUTextureFormat viewFormat = WGPUTextureFormat_Undefined);
	WGPUTextureView wgpCreateTextureView(const WGPUTexture& texture, WGPUTextureAspect aspect);
	WGPUSampler wgpCreateSampler(WGPUFilterMode filterMode = WGPUFilterMode_Linear, WGPUAddressMode addressMode = WGPUAddressMode_ClampToEdge, uint16_t maxAnisotropy = 1u, WGPUMipmapFilterMode mipmapFilterMode = WGPUMipmapFilterMode_Undefined, WGPUCompareFunction compareFunction = WGPUCompareFunction_Undefined);
	WGPUShaderModule wgpCreateShaderFromFile(std::string path);
	WGPUShaderModule wgpCreateShaderFromString(std::string strng);

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
	void wgpSetSurfaceColorFormat(WGPUTextureFormat textureFormat, const std::function<void()>& onSurfaceChange = NULL);
	void wgpSetMSAASampleCount(const uint32_t count, const std::function<void()>& onSurfaceChange = NULL);
}

enum SamplerSlot {
	SS_LINEAR_CLAMP,
	SS_LINEAR_REPEAT,
	SS_NEAREST_CLAMP,
	SS_NEAREST_REPEAT,
	SS_0,
	SS_1,
	SS_2
};

struct WgpContext {

	friend bool wgpCreateDevice(void* window);
	friend void wgpPipelinesRelease();
	friend void wgpSamplersRelease();
	friend void wgpShaderModulesRelease();
	friend void wgpPipelineLayoutsRelease();

	void createComputePipeline(std::string shaderModuleName, std::string pipelineLayoutName, const std::function<std::vector<WGPUBindGroupLayout>()>& onBindGroupLayouts);
	void createRenderPipeline(std::string shaderModuleName, 
		std::string pipelineLayoutName, 
		const VertexLayoutSlot vertexLayoutSlot, 
		const std::function<std::vector<WGPUBindGroupLayout>()>& onBindGroupLayouts = NULL, 
		uint32_t msaaSampleCount = 1u, 
		WGPUPrimitiveTopology primitiveTopology = WGPUPrimitiveTopology::WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat colorTextureFormat = WGPUTextureFormat::WGPUTextureFormat_Undefined,
		WGPUTextureFormat depthTextureFormat = WGPUTextureFormat::WGPUTextureFormat_Undefined,
		WGPUCompareFunction depthCompareFunction = WGPUCompareFunction::WGPUCompareFunction_Less,
		bool addDepthStencilState = true,
		bool addBlendState = true,
		bool addFragmentState = true);

	void createVertexBufferLayout(VertexLayoutSlot slot = VL_PTN);
	void addSampler(const WGPUSampler& sampler, SamplerSlot samplerSlot);
	const WGPUSampler& getSampler(SamplerSlot samplerSlot);
	void addSahderModule(const std::string& shaderModuleName, const std::string& stringPath, bool fromString = false);
	const WGPUShaderModule& getShaderModule(std::string shaderModuleName);
	void setClearColor(const WGPUColor& clearColor);
	
	WGPUInstance instance = NULL;
	WGPUAdapter adapter = NULL;
	WGPUDevice device = NULL;
	WGPUSurface surface = NULL;
	WGPUQueue queue = NULL;
	WGPUColor clearColor = { 0.2f, 0.2f, 0.2f, 1.0f };
	
	WGPUTextureView depthTextureView = NULL;
	WGPUTexture depthTexture = NULL;
	WGPUTextureView msaaTextureView = NULL;
	WGPUTexture msaaTexture = NULL;
	uint32_t msaaSampleCount = 1u;

	WGPUSurfaceConfiguration config = {};
	WGPUSurfaceCapabilities surfaceCapabilities;
	WGPUTextureFormat depthformat = WGPUTextureFormat::WGPUTextureFormat_Depth24PlusStencil8;
	WGPUTextureFormat colorformat = WGPUTextureFormat::WGPUTextureFormat_BGRA8UnormSrgb;

	std::unordered_map<std::string, WGPUComputePipeline> computePipelines;
	std::unordered_map<std::string, WGPURenderPipeline> renderPipelines;
	std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw = NULL;

private:

	std::unordered_map<std::string, WGPUPipelineLayout> pipelineLayouts;
	std::unordered_map<SamplerSlot, WGPUSampler> samplers;
	std::unordered_map<std::string, WGPUShaderModule> shaderModules;
};