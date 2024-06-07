#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <Physics/Physics.h>

#include "Chunk.h"

struct PhysicsChunk{
	btRigidBody* rigidMeshChunk;
	bool active;
	float X_origin;
	float Z_origin;

	PhysicsChunk(bool isActive, btRigidBody* mesh) : active(isActive), rigidMeshChunk(mesh) {}
};

class PhysicsChunkManager{

public:
	PhysicsChunkManager(std::vector<ChunkNew> chunks, float localScale = 1.0f);
	~PhysicsChunkManager();

	void update(btScalar playerX, btScalar playerZ);

private:
	std::vector<std::unique_ptr<PhysicsChunk>> chunkVector;
};