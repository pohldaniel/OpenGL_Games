#pragma once

#include <vector>
#include <memory>
#include <Physics/Physics.h>

#include "Chunk.h"

struct PhysicsChunk{
	std::unique_ptr<btRigidBody> rigidMeshChunk;
	bool active;
	float X_origin;
	float Z_origin;
};

class PhysicsChunkManager{

public:
	PhysicsChunkManager() = default;
	~PhysicsChunkManager();
	void update(btScalar playerX, btScalar playerZ);
	void init(std::vector<Chunk> chunks, float localScale = 1.0f);

private:

	std::vector<PhysicsChunk> m_chunks;
};