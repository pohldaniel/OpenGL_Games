#include "PhysicsChunkManager.h"

PhysicsChunkManager::PhysicsChunkManager(const std::vector<float>& verts, const std::string& filename) {
	SCALE_FACTOR = 40.0f;
	std::vector<LoadedChunk> chunks = ChunkedMapLoader::loadChunks(filename);

	glm::mat4 chunkModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f)); // Placeholder Model Matrix

	for (auto& ldChunk : chunks) {

		btTriangleMesh* mesh = new btTriangleMesh();

		for (size_t i = 0; i < ldChunk.faces.size(); i += 3){
			int VERT_LEN = 11;
			unsigned int idx0 = ldChunk.faces[i]; unsigned int idx1 = ldChunk.faces[i + 1]; unsigned int idx2 = ldChunk.faces[i + 2];
			btVector3 vertex0(verts[idx0 * VERT_LEN], verts[idx0 * VERT_LEN + 1], verts[idx0 * VERT_LEN + 2]);
			btVector3 vertex1(verts[idx1 * VERT_LEN], verts[idx1 * VERT_LEN + 1], verts[idx1 * VERT_LEN + 2]);
			btVector3 vertex2(verts[idx2 * VERT_LEN], verts[idx2 * VERT_LEN + 1], verts[idx2 * VERT_LEN + 2]);
			mesh->addTriangle(vertex0, vertex1, vertex2);
		}

		btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(mesh, true);
		shape->setLocalScaling(btVector3(40.0f, 40.0f, 40.0f));

		btTransform staticMeshTransform;
		staticMeshTransform.setIdentity();
		btDefaultMotionState* motion = new btDefaultMotionState(staticMeshTransform);
		btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, shape);
		info.m_friction = 2.0f;
		std::unique_ptr<PhysicsChunk> newChunk = std::make_unique<PhysicsChunk>(false, new btRigidBody(info));
		newChunk->X_origin = ldChunk.X_origin;
		newChunk->Z_origin = ldChunk.Z_origin;

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
		btScalar distanceX = playerX - (chunk.X_origin)*SCALE_FACTOR;
		btScalar distanceZ = playerZ - (chunk.Z_origin)*SCALE_FACTOR;
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