#pragma once
#include <webgpu.h>

class WgpBuffer {

public:

	WgpBuffer();
	WgpBuffer(WgpBuffer const& rhs);
	WgpBuffer(WgpBuffer&& rhs) noexcept;
	~WgpBuffer();

	void createBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage);
	void createBuffer(uint32_t size, WGPUBufferUsage bufferUsage);
	void cleanup();
	void markForDelete();

	WGPUBuffer m_buffer;
	bool m_markForDelete;
};