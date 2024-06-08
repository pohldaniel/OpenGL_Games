#include "PhysicsChunkManager.h"
#include "Globals.h"

void PhysicsChunkManager::init(std::vector<Chunk> chunks, float localScale) {
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
		m_chunks.push_back({ std::make_unique<btRigidBody>(info), false, chunk.m_centerX * localScale, chunk.m_centerZ * localScale });
	}
}

void PhysicsChunkManager::update(btScalar playerX, btScalar playerZ) {
	btDiscreteDynamicsWorld* physicsWorld = Physics::GetDynamicsWorld();

	// Define a radius within which chunks should be active
	constexpr btScalar activationRadius = 55.0;

	for (auto&& chunk : m_chunks) {
		// Calculate distance from player to chunk origin
		btScalar distanceX = playerX - chunk.X_origin;
		btScalar distanceZ = playerZ - chunk.Z_origin;
		btScalar distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);

		if (distance <= activationRadius && !chunk.active) {
			// Activate chunk and add its rigid body to the physics world
			chunk.active = true;
			physicsWorld->addRigidBody(chunk.rigidMeshChunk.get());

		}else if (distance > activationRadius && chunk.active) {

			// Deactivate chunk and remove its rigid body from the physics world
			chunk.active = false;
			physicsWorld->removeRigidBody(chunk.rigidMeshChunk.get());
		}
	}
}

PhysicsChunkManager::~PhysicsChunkManager() {

	btDiscreteDynamicsWorld* physicsWorld = Physics::GetDynamicsWorld();
	for (auto&& chunk : m_chunks) {
		if (chunk.rigidMeshChunk.get()->getMotionState()) {
			delete chunk.rigidMeshChunk.get()->getMotionState();
			chunk.rigidMeshChunk.get()->setMotionState(nullptr);
		}

		if (chunk.rigidMeshChunk.get()->getCollisionShape()) {
			delete chunk.rigidMeshChunk.get()->getCollisionShape();
			chunk.rigidMeshChunk.get()->setCollisionShape(nullptr);
		}

		if (chunk.active) {
			physicsWorld->removeRigidBody(chunk.rigidMeshChunk.get());
		}
	}
	m_chunks.clear();
}