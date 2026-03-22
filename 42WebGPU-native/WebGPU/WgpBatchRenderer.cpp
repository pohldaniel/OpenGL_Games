#include "WgpBatchRenderer.h"

WgpBatchRenderer WgpBatchRenderer::s_instance;

WgpBatchRenderer::~WgpBatchRenderer() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpBatchRenderer::cleanup() {
	delete[] buffer;
	buffer = nullptr;
	bufferPtr = nullptr;
}

void WgpBatchRenderer::markForDelete() {
	m_vertexBuffer.markForDelete();
	m_indexBuffer.markForDelete();
	m_markForDelete = true;
}

void WgpBatchRenderer::init(size_t size) {
	m_maxQuad = size;
	m_maxVert = m_maxQuad * 4u;
	m_maxIndex = m_maxQuad * 6u;

	m_vertexBuffer.createBuffer(sizeof(Vertex) * m_maxVert, WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);


	uint32_t* indices = new uint32_t[m_maxIndex];
	uint32_t index_offset = 0;
	for (int i = 0; i < m_maxIndex; i += 6) {
		indices[i + 0] = 0 + index_offset;
		indices[i + 1] = 1 + index_offset;
		indices[i + 2] = 2 + index_offset;

		indices[i + 3] = 2 + index_offset;
		indices[i + 4] = 3 + index_offset;
		indices[i + 5] = 0 + index_offset;

		index_offset += 4;
	}

	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indices), sizeof(uint32_t) * m_maxIndex, WGPUBufferUsage_Index | WGPUBufferUsage_Storage);
	delete[] indices;
	markForDelete();
}

void WgpBatchRenderer::addQuadAA(std::array<float, 4> posSize, std::array<float, 4> texPosSize, std::array<float, 4> color, uint32_t frame) {
	if (indexCount >= m_maxIndex) {
		//draw();
	}

	bufferPtr->posTex = { posSize[0], posSize[1],  texPosSize[0],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2], posSize[1],  texPosSize[0] + texPosSize[2],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2], posSize[1] + posSize[3],  texPosSize[0] + texPosSize[2],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0], posSize[1] + posSize[3],  texPosSize[0],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 6;
}

void WgpBatchRenderer::draw(const WGPURenderPassEncoder& renderPassEncoder){
	ptrdiff_t size = (uint8_t*)bufferPtr - (uint8_t*)buffer;

	for (uint32_t i = 0u; i < m_bindGroups.size(); i++) {
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, m_bindGroups[i], 0u, NULL);
	}
	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.getBuffer(), 0u, wgpuBufferGetSize(m_vertexBuffer.getBuffer()));
	wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.getBuffer()));
	wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indexCount, 1u, 0u, 0u, 0u);

	indexCount = 0;
	bufferPtr = buffer;
}

WgpBatchRenderer& WgpBatchRenderer::Get() {
	return s_instance;
}