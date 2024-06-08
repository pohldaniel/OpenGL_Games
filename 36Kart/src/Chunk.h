#pragma once
#include <iostream>
#include <vector>
#include <engine/MeshObject/Shape.h>
#include <engine/Vector.h>

struct Chunk {
	Chunk(float x, float z, float radius, float scale, const std::vector<Vector3f>& verts, const std::vector<unsigned int>& indices);

	std::vector<float> m_verts;
	float m_centerX;
	float m_centerZ;
	static std::vector<Chunk> Chunks;

	static void LoadChunks(const Shape& shape);
	static void ClearChunks();
};