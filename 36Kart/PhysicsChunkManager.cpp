#include "PhysicsChunkManager.h"

PhysicsChunkManager::PhysicsChunkManager(std::vector<ChunkNew> chunks, float localScale) {
	for (auto&& chunk : chunks) {
		btTriangleMesh* mesh = new btTriangleMesh();
		for (size_t i = 0; i < chunk.m_verts.size() / 9; i++) {
			btVector3 vertex0(chunk.m_verts[i * 9], chunk.m_verts[i * 9 + 1], chunk.m_verts[i * 9 + 2]);
			btVector3 vertex1(chunk.m_verts[i * 9 + 3], chunk.m_verts[i * 9 + 4], chunk.m_verts[i * 9 + 5]);
			btVector3 vertex2(chunk.m_verts[i * 9 + 6], chunk.m_verts[i * 9 + 7], chunk.m_verts[i * 9 + 8]);
			mesh->addTriangle(vertex0, vertex1, vertex2);
		}

		btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(mesh, true);
		shape->setLocalScaling(btVector3(localScale, localScale, localScale));

		btTransform staticMeshTransform;
		staticMeshTransform.setIdentity();
		btDefaultMotionState* motion = new btDefaultMotionState(staticMeshTransform);
		btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, shape);
		info.m_friction = 2.0f;
		std::unique_ptr<PhysicsChunk> newChunk = std::make_unique<PhysicsChunk>(false, new btRigidBody(info));
		newChunk->X_origin = chunk.m_centerX * localScale;
		newChunk->Z_origin = chunk.m_centerZ * localScale;
		chunkVector.push_back(std::move(newChunk));
	}
}

void PhysicsChunkManager::update(btScalar playerX, btScalar playerZ) {

	btDiscreteDynamicsWorld* physicsWorld = Physics::GetDynamicsWorld();

	// Define a radius within which chunks should be active
	constexpr btScalar activationRadius = 55.0;

	for (auto& chunkUniquePtr : chunkVector) {

		PhysicsChunk& chunk = *chunkUniquePtr;

		// Calculate distance from player to chunk origin
		btScalar distanceX = playerX - chunk.X_origin;
		btScalar distanceZ = playerZ - chunk.Z_origin;
		btScalar distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);

		if (distance <= activationRadius && !chunk.active) {
			// Activate chunk and add its rigid body to the physics world
			chunk.active = true;
			physicsWorld->addRigidBody(chunk.rigidMeshChunk);

		}else if (distance > activationRadius && chunk.active) {

			// Deactivate chunk and remove its rigid body from the physics world
			chunk.active = false;
			physicsWorld->removeRigidBody(chunk.rigidMeshChunk);
		}
	}
}

PhysicsChunkManager::~PhysicsChunkManager() {
	chunkVector.clear();
}