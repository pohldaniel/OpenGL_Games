#include <WebGPU/WgpContext.h>
#include "WgpMesh.h"

WgpMesh::WgpMesh(const WgpBuffer& wgpBufferVertex, const WgpBuffer& wgpBufferIndex, const WgpTexture& wgpTexture, const uint32_t drawCount) : wgpBufferVertex(wgpBufferVertex), wgpBufferIndex(wgpBufferIndex), wgpTexture(wgpTexture), drawCount(drawCount) {

}

WgpMesh::WgpMesh(WgpMesh const& rhs) : wgpBufferVertex(rhs.wgpBufferVertex), wgpBufferIndex(rhs.wgpBufferIndex), wgpTexture(rhs.wgpTexture), drawCount(rhs.drawCount) {

}

WgpMesh::WgpMesh(WgpMesh&& rhs) noexcept : wgpBufferVertex(rhs.wgpBufferVertex), wgpBufferIndex(rhs.wgpBufferIndex), wgpTexture(rhs.wgpTexture), drawCount(rhs.drawCount) {

}

void WgpMesh::draw(const WGPURenderPassEncoder& renderPass) const {
	//wgpuQueueWriteBuffer(wgpContext.queue, wgpBufferVertex.m_buffer, 0, vertexData.data(), vertexData.size() * sizeof(VertexAttributes));
}