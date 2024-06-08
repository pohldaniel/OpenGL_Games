#include "PhysicsChunkManager.h"
#include "Globals.h"

void PhysicsChunkManager::init(std::vector<Chunk> chunks, float localScale) {
	for (auto&& chunk : chunks) {		
		btCollisionShape *shape = Physics::CreateCollisionShape(chunk.m_verts);

		btTransform transform;
		transform.setIdentity();
		btDefaultMotionState* motion = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, shape);
		info.m_friction = 2.0f;
		m_chunks.push_back({ std::make_unique<btRigidBody>(info), false, chunk.m_centerX * localScale, chunk.m_centerZ * localScale });
		m_chunks.back().rigidMeshChunk->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
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
			physicsWorld->addRigidBody(chunk.rigidMeshChunk.get(), Physics::FLOOR, Physics::CAR);

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