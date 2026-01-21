#pragma once
#include <webgpu.h>

class WgpBuffer {

	friend class WgpMesh;
	friend class WgpModel;

public:

	WgpBuffer();
	WgpBuffer(WgpBuffer const& rhs);
	WgpBuffer(WgpBuffer&& rhs) noexcept;
	~WgpBuffer();

	void createBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage);
	void createBuffer(uint32_t size, WGPUBufferUsage bufferUsage);
	void cleanup();
	void markForDelete();
	const WGPUBuffer& getWgpuBuffer() const;

private:

	WGPUBuffer m_buffer;
	bool m_markForDelete;
};