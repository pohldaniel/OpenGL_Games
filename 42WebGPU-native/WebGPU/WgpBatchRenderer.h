#pragma once
#include <array>
#include <vector>
#include <functional>
#include "WgpBuffer.h"

class WgpBatchRenderer {

	struct Vertex {
		std::array<float, 5> posTex;
		std::array<float, 4> color;
		unsigned int layer;
	};

	struct Batch {
		Vertex* buffer;
		Vertex* bufferPtr;
	};

public:
	
	WgpBatchRenderer();
	~WgpBatchRenderer();

	void draw(const WGPURenderPassEncoder& renderPassEncoder);

	void init(size_t size = 400u, size_t maxBatches = 4u);
	void addQuadAA(const std::array<float, 4>& posSize, const std::array<float, 4>& texCoords = { 0.0f, 0.0f, 1.0f, 1.0f }, const std::array<float, 4>& color = { 1.0f, 1.0f, 1.0f, 1.0f }, uint32_t layer = 0u);
	void addQuad(const std::array<std::array<float,3>,4>& vertices, const std::array<float, 4>& texCoords = {0.0f, 0.0f, 1.0f, 1.0f}, const std::array<float, 4>& color = {1.0f, 1.0f, 1.0f, 1.0f}, uint32_t layer = 0u);
	void setBindGroups(const std::function<std::vector<WGPUBindGroup>()>& onBindGroups);

	void cleanup();
	void markForDelete();
	void reset();

	static WgpBatchRenderer& Get();

private:

	size_t m_maxQuad;
	size_t m_maxVert;
	size_t m_maxIndex;
	size_t m_maxBatches;

	uint32_t m_indexCount;
	uint32_t m_currentBatch;

	bool m_markForDelete;

	std::vector<Batch> m_batches;
	std::vector <WgpBuffer> m_vertexBuffers;
	WgpBuffer m_indexBuffer;
	mutable std::vector<WGPUBindGroup> m_bindGroups;

	static WgpBatchRenderer s_instance;
};