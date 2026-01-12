
#include <utility>
#include "WgpContext.h"
#include "WgpBuffer.h"

WgpBuffer::WgpBuffer() : m_buffer(NULL) {

}

WgpBuffer::WgpBuffer(WgpBuffer const& rhs) : m_buffer(NULL) {

}

WgpBuffer::WgpBuffer(WgpBuffer&& rhs) noexcept : m_buffer(std::move(rhs.m_buffer)) {

}

void WgpBuffer::createBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage) {
	m_buffer = wgpCreateBuffer(size, bufferUsage);
	//wgpuQueueWriteBuffer(Queue, vertexBuffer, 0, vertexData.data(), bufferDesc.size);
}