#pragma once

#include <array>
#include <string>
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

	WGPUInstance instance;
	WGPUSurface surface;

	WGPUAdapter adapter;
	WGPUDevice device;
	WGPUQueue queue;

	WGPUBindGroupLayout bindGroupLayout;
};

extern WgpContext wgpContext;


extern "C" {
	void wgpInit(void* window);
	bool wgpCreateDevice(WgpContext& wgpContext, void* window);

	WGPUPipelineLayout wgpCreatePipelineLayout();
	WGPUBuffer wgpCreateBuffer(uint32_t size, WGPUBufferUsage bufferUsage);
}