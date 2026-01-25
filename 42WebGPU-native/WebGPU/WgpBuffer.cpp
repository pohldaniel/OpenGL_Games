#include <utility>
#include "WgpContext.h"
#include "WgpBuffer.h"

WgpBuffer::WgpBuffer() : m_buffer(NULL), m_markForDelete(false) {

}

WgpBuffer::WgpBuffer(WgpBuffer const& rhs) : m_buffer(rhs.m_buffer), m_markForDelete(false) {

}

WgpBuffer::WgpBuffer(WgpBuffer&& rhs) noexcept : m_buffer(std::move(rhs.m_buffer)), m_markForDelete(false) {

}

WgpBuffer::~WgpBuffer() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpBuffer::cleanup() {
	wgpuBufferDestroy(m_buffer);
	wgpuBufferRelease(m_buffer);
	m_buffer = NULL;
}

void WgpBuffer::markForDelete() {
	m_markForDelete = true;
}

void WgpBuffer::createBuffer(uint32_t size, WGPUBufferUsage bufferUsage) {
	m_buffer = wgpCreateEmptyBuffer(size, bufferUsage);
}

void WgpBuffer::createBuffer(const void* data, uint32_t size, WGPUBufferUsage bufferUsage) {
	m_buffer = wgpCreateBuffer(data, size, bufferUsage);
}

const WGPUBuffer& WgpBuffer::getWgpuBuffer() const {
	return m_buffer;
}