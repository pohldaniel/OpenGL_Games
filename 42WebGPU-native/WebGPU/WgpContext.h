#pragma once

#include <array>
#include <string>
#include <functional>
#include <webgpu.h>
#include <engine/Vector.h>

struct MyUniforms {
	Matrix4f projectionMatrix;
	Matrix4f viewMatrix;
	Matrix4f modelMatrix;
	std::array<float, 4> color;
	float time;
	float _pad[3];
};

enum RenderPipelineSlot {
	RP_PTN,
	RP_WIREFRAME
};

struct WgpContext {

	WGPURenderPipeline createRenderPipelinePTN(std::string path, std::function<WGPUPipelineLayout(WgpContext& context)> OnPipelineLayout);
	static WGPUPipelineLayout CreatePipelineLayout(WgpContext& context);

	WGPUInstance instance = nullptr;
	WGPUSurface surface = nullptr;

	WGPUAdapter adapter = nullptr;
	WGPUDevice device = nullptr;
	WGPUQueue queue = nullptr;
	WGPUSurfaceConfiguration config = {};
	WGPUSurfaceCapabilities surfaceCapabilities;

	std::unordered_map<RenderPipelineSlot, WGPURenderPipeline> renderPipelines;


	WGPUTextureFormat depthformat = WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;
	WGPUBindGroupLayout bindGroupLayout;

	WGPUBuffer uniformBuffer;
	WGPUBindGroup bindGroup;
	MyUniforms uniforms;
	float angle1 = 2.0f;
	Matrix4f S;
	Matrix4f T1;
	Matrix4f R1;
	Matrix4f R2;
	Matrix4f T2;

	
	
	
	WGPUTextureView depthTextureView;
	WGPUTexture depthTexture;

	//WGPUTextureView textureView;
	WGPUTexture texture;


	std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw;
};

enum VertexAttributeSlot {
	VA_P,
	VA_PT,
	VA_PN,
	VA_PTN,
};

enum VertexBufferLayoutSlot {
	VBL_P,
	VBL_PT,
	VBL_PN,
	VBL_PTN,
};

extern WgpContext wgpContext;
extern std::unordered_map<VertexAttributeSlot, std::vector<WGPUVertexAttribute>> wgpVertexAttributes;
extern std::unordered_map<VertexBufferLayoutSlot, WGPUVertexBufferLayout> wgpVertexBufferLayouts;

extern "C" {
	void wgpInit(void* window);
	bool wgpCreateDevice(WgpContext& wgpContext, void* window);

	WGPUBuffer wgpCreateBuffer(uint32_t size, WGPUBufferUsage bufferUsage);

	WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, WGPUTextureFormat viewFormat = WGPUTextureFormat_Undefined);		
	WGPUTextureView wgpCreateTextureView(WGPUTextureFormat textureFormat, WGPUTextureAspect aspect, const WGPUTexture& texture);
	WGPUShaderModule wgpCreateShader(std::string path);
	void wgpCreateVertexBufferLayout(std::unordered_map<VertexAttributeSlot, std::vector<WGPUVertexAttribute>>& vertexAttributes, std::unordered_map<VertexBufferLayoutSlot, WGPUVertexBufferLayout>& vertexBufferLayouts, VertexBufferLayoutSlot slot = VBL_PTN);


	void wgpDraw();
	void wgpResize(uint32_t width, uint32_t height);
	void wgpToggleVerticalSync();
}