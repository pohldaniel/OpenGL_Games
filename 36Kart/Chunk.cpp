#include "Chunk.h"

std::vector<ChunkNew> ChunkNew::Chunks;

ChunkNew::ChunkNew(float x, float z, float radius, float scale, const std::vector<Vector3f>& verts, const std::vector<unsigned int>& indices) {
	for (size_t i = 0; i < indices.size() / 3; i++) {
		int id0 = indices[i * 3];
		const Vector3f& a = verts[id0];

		int id1 = indices[i * 3 + 1];
		const Vector3f& b = verts[id1];

		int id2 = indices[i * 3 + 2];
		const Vector3f& c = verts[id2];

		float maxA = std::max(fabs(a[0] - x), fabs(a[2] - z));
		float maxB = std::max(fabs(b[0] - x), fabs(b[2] - z));
		float maxC = std::max(fabs(c[0] - x), fabs(c[2] - z));

		if (maxA < radius && maxB < radius && maxC < radius) {
			m_verts.push_back(a[0] * scale);
			m_verts.push_back(a[1] * scale);
			m_verts.push_back(a[2] * scale);

			m_verts.push_back(b[0] * scale);
			m_verts.push_back(b[1] * scale);
			m_verts.push_back(b[2] * scale);

			m_verts.push_back(c[0] * scale);
			m_verts.push_back(c[1] * scale);
			m_verts.push_back(c[2] * scale);
		}
	}

	m_centerX = x * scale;
	m_centerZ = z * scale;
}

void ChunkNew::LoadChunks(const Shape& shape) {
	const BoundingBox& aabb = shape.getAABB();
	const Vector3f& size = aabb.getSize();

	float x = size[0] * 0.1f;
	float z = size[2] * 0.1f;

	for (size_t i = 0; i < 10; i++) {
		for (size_t j = 0; j < 10; j++) {
			Chunks.push_back(ChunkNew(aabb.min[0] + ((float)i) * x, aabb.min[2] + ((float)j) * z, 1.49f, 40.0f, shape.getPositions(), shape.getIndexBuffer()));
		}
	}
}