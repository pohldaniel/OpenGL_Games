#pragma once
#include <vector>
#include <btBulletDynamicsCommon.h>
#include "engine/Vector.h"


class Mesh;
class ObjModel;
class MeshCube;
class MeshQuad;
class Terrain;
class PhysicsCar;

class btFilteredVehicleRaycaster : public btVehicleRaycaster{
	btDynamicsWorld* m_dynamicsWorld;

public:
	btFilteredVehicleRaycaster(btDynamicsWorld* world, int collisionFilterGroup, int collisionFilterMask)
		: m_dynamicsWorld(world)
	{
		m_collisionFilterGroup = collisionFilterGroup;
		m_collisionFilterMask = collisionFilterMask;
	}

	virtual void* castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result) override;
private:
	int m_collisionFilterGroup = -1;
	int m_collisionFilterMask = -1;
};

class Physics{
public:

	enum collisiontypes {
		TERRAIN = 1,
		RENDERABLE_OBJECT = 2,
		RAY = 4,
		PICKABLE_OBJECT = 8,
		CAR = 16,
		COL_FORCE_8BIT = 0xFF
	};


	Physics(float pysicsStep);
	~Physics(void);

	void initialize();
	void deinitialize();

	void stepSimulation(btScalar timeStep);
	btRigidBody * addRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	btRigidBody* addStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw = true, const btVector3 & scale = btVector3(1, 1, 1), int collisionFilterGroup = 1, int collisionFilterMask = -1);

	void bebugDrawWorld() { m_dynamicsWorld->debugDrawWorld(); }

	static btCollisionShape* CreateStaticCollisionShape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, float scale = 1.f);
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale = btVector3(1, 1, 1));

	static btCollisionShape* CreateStaticCollisionShape(MeshCube* mesh, const btVector3 & scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(MeshCube* model, float scale = 1.f);

	static btCollisionShape* CreateStaticCollisionShape(MeshQuad* mesh, const btVector3 & scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(MeshQuad* model, float scale = 1.f);

	static btCollisionShape * CreateStaticCollisionShape(Terrain* mesh, const btVector3 & scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape *> CreateStaticCollisionShapes(Terrain* model, float scale = 1.f);

	static btCollisionShape* CreateStaticCollisionShape(Mesh* mesh, const btVector3& scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(ObjModel * model, const btVector3 & scale);
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(ObjModel * model, float scale = 1.f);
	static btTransform BtTransform();
	static btTransform BtTransform(const Vector3f& origin);
	static btTransform BtTransform(const Vector3f& axis, float degrees);
	static btTransform BtTransform(const Vector3f& origin, const Vector3f& axis, float degrees);

	static Matrix4f MatrixFrom(const btTransform& trans, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));	
	static Matrix4f MatrixTransposeFrom(const btTransform& trans, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));

	static btRigidBody* CreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape);
	

	btDiscreteDynamicsWorld * GetDynamicsWorld() { return m_dynamicsWorld; }
	
private:

	btRigidBody * createRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape);

	btCollisionDispatcher* m_dispatcher;
	//btBroadphaseInterface* m_overlappingPairCache;
	btBroadphaseInterface* m_broadphase;
	btSequentialImpulseConstraintSolver* m_constraintSolver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;

	btDiscreteDynamicsWorld *m_dynamicsWorld;

	float m_physicsStep;
};