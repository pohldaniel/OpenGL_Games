#include <WebGPU/WgpContext.h>
#include "WgpBatchRenderer.h"

WgpBatchRenderer WgpBatchRenderer::s_instance;

WgpBatchRenderer::WgpBatchRenderer() : 
	m_maxQuad(0u),
	m_maxVert(0u),
	m_maxIndex(0u),
	m_maxBatches(0u),
	m_indexCount(0u),
	m_currentBatch(0u),
	m_markForDelete(false) {
}

WgpBatchRenderer::~WgpBatchRenderer() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpBatchRenderer::cleanup() {
	for (Batch& batch : m_batches) {
		delete[] batch.buffer;
		batch.buffer = nullptr;
		batch.bufferPtr = nullptr;
	}
}

void WgpBatchRenderer::markForDelete() {
	m_vertexBuffer.markForDelete();
	m_indexBuffer.markForDelete();
	m_markForDelete = true;
}

void WgpBatchRenderer::init(size_t size, size_t batches) {

	m_maxBatches = batches;
	m_maxQuad = size;
	m_maxVert = m_maxQuad * 4u;
	m_maxIndex = m_maxQuad * 6u;

	m_vertexBuffer.createBuffer(sizeof(Vertex) * m_maxVert, WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);


	uint32_t* indices = new uint32_t[m_maxIndex];
	uint32_t index_offset = 0u;
	for (size_t i = 0u; i < m_maxIndex; i += 6u) {
		indices[i + 0] = 0u + index_offset;
		indices[i + 1] = 1u + index_offset;
		indices[i + 2] = 2u + index_offset;

		indices[i + 3] = 2u + index_offset;
		indices[i + 4] = 3u + index_offset;
		indices[i + 5] = 0u + index_offset;

		index_offset += 4u;
	}

	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indices), sizeof(uint32_t) * m_maxIndex, WGPUBufferUsage_Index | WGPUBufferUsage_Storage);
	delete[] indices;
	markForDelete();

	m_batches.resize(m_maxBatches);
	for (size_t index = 0u; index < m_maxBatches; index++) {
		m_batches[index].buffer = new Vertex[m_maxVert];
		m_batches[index].bufferPtr = m_batches[index].buffer;
	}
}

void WgpBatchRenderer::setBindGroups(const std::function<std::vector<WGPUBindGroup>()>& onBindGroups) {
	m_bindGroups = onBindGroups();
}

void WgpBatchRenderer::addQuadAA(std::array<float, 4> posSize, std::array<float, 4> texCoords, std::array<float, 4> color, uint32_t layer) {
	if (m_indexCount >= m_maxIndex) {
		m_currentBatch++;
		m_indexCount = 0u;
	}

	m_batches[m_currentBatch].bufferPtr->posTex = { posSize[0], posSize[1],  texCoords[0],  texCoords[1] };
	m_batches[m_currentBatch].bufferPtr->color = { color[0], color[1], color[2], color[3] };
	m_batches[m_currentBatch].bufferPtr->layer = layer;
	m_batches[m_currentBatch].bufferPtr++;

	m_batches[m_currentBatch].bufferPtr->posTex = { posSize[0] + posSize[2], posSize[1],  texCoords[0] + texCoords[2],  texCoords[1] };
	m_batches[m_currentBatch].bufferPtr->color = { color[0], color[1], color[2], color[3] };
	m_batches[m_currentBatch].bufferPtr->layer = layer;
	m_batches[m_currentBatch].bufferPtr++;

	m_batches[m_currentBatch].bufferPtr->posTex = { posSize[0] + posSize[2], posSize[1] + posSize[3],  texCoords[0] + texCoords[2],  texCoords[1] + texCoords[3] };
	m_batches[m_currentBatch].bufferPtr->color = { color[0], color[1], color[2], color[3] };
	m_batches[m_currentBatch].bufferPtr->layer = layer;
	m_batches[m_currentBatch].bufferPtr++;

	m_batches[m_currentBatch].bufferPtr->posTex = { posSize[0], posSize[1] + posSize[3],  texCoords[0],  texCoords[1] + texCoords[3] };
	m_batches[m_currentBatch].bufferPtr->color = { color[0], color[1], color[2], color[3] };
	m_batches[m_currentBatch].bufferPtr->layer = layer;
	m_batches[m_currentBatch].bufferPtr++;

	m_indexCount += 6;
}

void WgpBatchRenderer::draw(const WGPURenderPassEncoder& renderPassEncoder) {

	for (uint32_t index = 0u; index  < m_currentBatch + 1u; index++) {
		ptrdiff_t size = (uint8_t*)m_batches[index].bufferPtr - (uint8_t*)m_batches[index].buffer;

		wgpuQueueWriteBuffer(wgpContext.queue, m_vertexBuffer.getBuffer(), 0u, reinterpret_cast<void*>(m_batches[index].buffer), size);


		uint32_t indexCount = (size / (4u * sizeof(Vertex))) * 6u;

		for (uint32_t i = 0u; i < m_bindGroups.size(); i++) {
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, m_bindGroups[i], 0u, NULL);
		}
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.getBuffer(), 0u, wgpuBufferGetSize(m_vertexBuffer.getBuffer()));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.getBuffer()));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indexCount, 1u, 0u, 0u, 0u);

		m_batches[index].bufferPtr = m_batches[index].buffer;
	}

	m_currentBatch = 0u;
	m_indexCount = 0u;
}

WgpBatchRenderer& WgpBatchRenderer::Get() {
	return s_instance;
}