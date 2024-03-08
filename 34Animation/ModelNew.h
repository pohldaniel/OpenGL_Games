#pragma once

#include <vector>
#include <array>
#include <engine/Vector.h>
#include <Utils/SolidIO.h>

class ModelNew {

	struct Vertex {
		Vector3f pos;
		Vector2f tex;
	};

public:

	ModelNew();
	void draw();


	short m_muscleNum, m_vertexNum;
	Matrix4f* m_skinMatrices;
	Utils::Skeleton m_skeleton;
	
	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBufferDraw;	
	std::vector<float> m_vertexBuffer;
	std::vector<std::array<float, 4>> m_weights;
	std::vector<std::array<unsigned int, 4>> m_boneIds;
	std::vector<Utils::SolidIO::Vertex> m_vertexBufferMap;

	unsigned int m_vao = 0u;
	unsigned int m_vbo[3] = { 0u };
	unsigned int m_drawCount;

	Utils::SolidIO solidConverter;

	Vertex* buffer;
	Vertex* bufferPtr;
};