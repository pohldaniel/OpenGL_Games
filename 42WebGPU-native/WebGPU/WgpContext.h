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

struct WgpContext {

	WGPUShaderModule createShader(const WGPUDevice& device);

	WGPUInstance instance = nullptr;
	WGPUSurface surface = nullptr;

	WGPUAdapter adapter = nullptr;
	WGPUDevice device = nullptr;
	WGPUQueue queue = nullptr;

	WGPUBindGroupLayout bindGroupLayout;

	WGPUTextureFormat depthformat = WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;


	WGPUBuffer uniformBuffer;
	WGPUBindGroup bindGroup;
	MyUniforms uniforms;
	float angle1 = 2.0f;
	Matrix4f S;
	Matrix4f T1;
	Matrix4f R1;
	Matrix4f R2;
	Matrix4f T2;

	WGPUSurfaceCapabilities surface_capabilities;
	WGPUSurfaceConfiguration config;
	WGPURenderPipeline pipeline;
	WGPUTextureView depthTextureView;
	WGPUTexture depthTexture;


	std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw;
};

extern WgpContext wgpContext;


extern "C" {
	void wgpInit(void* window);
	bool wgpCreateDevice(WgpContext& wgpContext, void* window);

	WGPUPipelineLayout wgpCreatePipelineLayout();
	WGPUBuffer wgpCreateBuffer(uint32_t size, WGPUBufferUsage bufferUsage);

	WGPUTexture wgpCreateTexture(uint32_t width, uint32_t height, WGPUTextureFormat textureFormat, WGPUTextureUsage textureUsage);
	WGPUTextureView wgpCreateTextureView(WGPUTextureFormat textureFormat, const WGPUTexture& texture);
	void wgpDraw();
}