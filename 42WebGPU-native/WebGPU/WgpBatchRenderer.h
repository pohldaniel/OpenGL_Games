#pragma once
#include <array>
#include <vector>
#include "WgpBuffer.h"

class WgpBatchRenderer {

	struct Vertex {
		std::array<float, 4> posTex;
		std::array<float, 4> color;
		unsigned int frame;
	};

public:

	WgpBatchRenderer() = default;
	~WgpBatchRenderer();

	void draw(const WGPURenderPassEncoder& renderPassEncoder);

	void init(size_t size = 400);
	void addQuadAA(std::array<float, 4> posSize, std::array<float, 4> texPosSize = { 0.0f, 0.0f, 1.0f, 1.0f }, std::array<float, 4> color = { 1.0f, 1.0f, 1.0f, 1.0f }, uint32_t frame = 0u);

	void cleanup();
	void markForDelete();

	static WgpBatchRenderer& Get();

private:

	size_t m_maxQuad = 0;
	size_t m_maxVert = 0;
	size_t m_maxIndex = 0;

	uint32_t indexCount = 0;

	Vertex* buffer;
	Vertex* bufferPtr;

	WgpBuffer m_vertexBuffer;
	WgpBuffer m_indexBuffer;
	bool m_markForDelete;
	mutable std::vector<WGPUBindGroup> m_bindGroups;

	static WgpBatchRenderer s_instance;
};