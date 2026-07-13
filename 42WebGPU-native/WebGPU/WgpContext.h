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
#define STRVIEW(str) WGPUStringView{ str, sizeof(str) - 1 }

enum VertexLayoutSlot {
	VL_NONE,
	VL_P,
	VL_PT,
	VL_PN,
	VL_PNC,
	VL_PTN,
	VL_PTNC,
	VL_PTNTB,
	VL_PTNWJ,
	VL_BATCH,
	VL_GUI,
	VL_0,
	VL_1,
	VL_2
};

struct WgpContext;
extern WgpContext wgpContext;
extern std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexAttribute>> wgpVertexAttributes;
extern std::unordered_map<VertexLayoutSlot, std::vector<WGPUVertexBufferLayout>> wgpVertexBufferLayouts;

extern "C" {
	void wgpInit(void* window);
	bool wgpCreateDevice(void* window);

	WGPUBuffer wgpCreateEmptyBuffer(uint32_t size, WGPUBufferUsage bufferUsage, bool mappedAtCreation = false);
	WGPUBuffer wgpCreateBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage);
	WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, uint32_t depth, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, uint32_t mipLevelCount = 1u, uint32_t sampleCount = 1u, WGPUTextureFormat viewFormat = WGPUTextureFormat_Undefined);
	WGPUTextureView wgpCreateTextureView(const WGPUTexture& texture, WGPUTextureAspect aspect);
	WGPUSampler wgpCreateSampler(WGPUFilterMode filterMode = WGPUFilterMode_Linear, WGPUAddressMode addressMode = WGPUAddressMode_ClampToEdge, uint16_t maxAnisotropy = 1u, WGPUMipmapFilterMode mipmapFilterMode = WGPUMipmapFilterMode_Undefined, WGPUCompareFunction compareFunction = WGPUCompareFunction_Undefined);
	WGPUShaderModule wgpCreateShaderFromFile(const std::string& path);
	WGPUShaderModule wgpCreateShaderFromString(const std::string& strng);
	std::vector<WGPUVertexAttribute>& wgpVertexAttribute(VertexLayoutSlot vertexLayoutSlot);
	std::vector<WGPUVertexBufferLayout>& wgpVertexBufferLayout(VertexLayoutSlot vertexLayoutSlot);

	void wgpCreateVertexBufferLayout(VertexLayoutSlot slot = VL_PTN);
	void wgpShutDown();
	void wgpCleanState();
	void wgpSamplersRelease();
	void wgpPipelinesRelease();
	void wgpShaderModulesRelease();
	void wgpPipelineLayoutsRelease();
	
	void wgpDraw();
	void wgpSubmitQueue();
	void wgpResize(uint32_t width, uint32_t height);
	void wgpToggleVerticalSync();
	void wgpConfigureSurface();
	void wgpSetSurfaceColorFormat(WGPUTextureFormat textureFormat, const std::function<void()>& onSurfaceChange = NULL);
	void wgpSetSurfaceDepthFormat(WGPUTextureFormat textureFormat, const std::function<void()>& onSurfaceChange = NULL);
	void wgpSetMSAASampleCount(const uint32_t count, const std::function<void()>& onSurfaceChange = NULL);	
	WGPURenderPassDepthStencilAttachment wgpCopyDepthStencilAttachment(const WGPURenderPassDepthStencilAttachment* src);
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

enum RenderPipelineFlags {
	WRITE_DEPTH = 1,
	DEPTH_STENCIL_STATE = 2,
	BLEND_STATE = 4,
	FRAGMENT_STATE = 8
};

enum BlendMode {
	ALPHA_BLENDING,
	ADDITIVE_BLENDING_SRC,
	ADDITIVE_BLENDING_ONE
};

enum StencilMode {
	DEFAULT,
	SET,
	MASK	
};

struct WgpContext {

	struct PipelineConfiguration {
		unsigned int flags;
		BlendMode blendMode;
		WGPUTextureFormat colorTextureFormat;
		WGPUCullMode cullMode;
		StencilMode stencilMode;
		std::vector<WGPUConstantEntry> constantEntries;
	};

	friend bool wgpCreateDevice(void* window);
	friend void wgpCleanState();
	friend void wgpSamplersRelease();
	friend void wgpPipelinesRelease();	
	friend void wgpShaderModulesRelease();
	friend void wgpPipelineLayoutsRelease();

	void createComputePipeline(const std::string& shaderModuleName,
		                       const std::string& entrypoint,
		                       const std::string& pipelineLayoutName,
		                       const std::function<std::vector<WGPUBindGroupLayout>()>& onBindGroupLayouts = NULL);

	void createRenderPipeline(const std::string& shaderModuleName,
		const std::string& pipelineLayoutName,
		const VertexLayoutSlot vertexLayoutSlot, 
		const std::function<std::vector<WGPUBindGroupLayout>()>& onBindGroupLayouts = NULL, 
		uint32_t msaaSampleCount = 1u, 
		WGPUPrimitiveTopology primitiveTopology = WGPUPrimitiveTopology::WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat colorTextureFormat = WGPUTextureFormat::WGPUTextureFormat_Undefined,
		WGPUTextureFormat depthTextureFormat = WGPUTextureFormat::WGPUTextureFormat_Undefined,
		WGPUCompareFunction depthCompareFunction = WGPUCompareFunction::WGPUCompareFunction_Less,	
		const PipelineConfiguration configuration = { WRITE_DEPTH | DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_Undefined, StencilMode::DEFAULT, {} });

	void addSampler(const WGPUSampler& sampler, SamplerSlot samplerSlot);
	const WGPUSampler& getSampler(SamplerSlot samplerSlot) const;
	void addSahderModule(const std::string& shaderModuleName, const std::string& stringPath, bool fromString = false);
	const WGPUShaderModule& getShaderModule(const std::string& shaderModuleName) const;
	const WGPUPipelineLayout& getPipelineLayout(const std::string& pipelineLayoutName) const;
	void setClearColor(const WGPUColor& clearColor);
	bool isBlendAble(WGPUTextureFormat textureFormat);
	
	WGPUInstance instance = NULL;
	WGPUAdapter adapter = NULL;
	WGPUDevice device = NULL;
	WGPUSurface surface = NULL;
	WGPUQueue queue = NULL;
	WGPUCommandEncoder commandEncoder = NULL;
	WGPUColor clearColor = { 0.2f, 0.2f, 0.2f, 1.0f };
	
	WGPUTextureView depthTextureView = NULL;
	WGPUTexture depthTexture = NULL;
	WGPUTextureView msaaTextureView = NULL;
	WGPUTexture msaaTexture = NULL;
	uint32_t msaaSampleCount = 1u;

	WGPUSurfaceConfiguration config = {};
	WGPUSurfaceCapabilities surfaceCapabilities;
	WGPUTextureFormat depthFormat = WGPUTextureFormat::WGPUTextureFormat_Depth24PlusStencil8;
	WGPUTextureFormat colorFormat = WGPUTextureFormat::WGPUTextureFormat_BGRA8UnormSrgb;

	std::unordered_map<std::string, WGPUComputePipeline> computePipelines;
	std::unordered_map<std::string, WGPURenderPipeline> renderPipelines;
	std::function<void(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor)> OnDraw = NULL;
	std::function<void()> OnPostDraw = NULL;

private:

	std::unordered_map<std::string, WGPUPipelineLayout> pipelineLayouts;
	std::unordered_map<std::string, WGPUShaderModule> shaderModules;
	std::unordered_map<SamplerSlot, WGPUSampler> samplers;
};