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
	void createBuffer(uint32_t size, WGPUBufferUsage bufferUsage, bool mappedAtCreation = false);
	void cleanup();
	void markForDelete() const;
	const WGPUBuffer& getBuffer() const;

private:

	WGPUBuffer m_buffer;
	mutable bool m_markForDelete;
};