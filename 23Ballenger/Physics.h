#pragma once
#include <vector>
#include <btBulletDynamicsCommon.h>
#include "engine/Vector.h"

class ObjMesh;
class ObjModel;
class Shape;
class Terrain;

class Physics{

public:

	enum collisiontypes {
		TERRAIN = 1,
		RENDERABLE_OBJECT = 2,
		RAY = 4,
		PICKABLE_OBJECT = 8,
		CAMERA = 16,
		CHARACTER = 32,
		TRIGGER = 64,
		COL_FORCE_8BIT = 0xFF
	};


	Physics(float pysicsStep);
	~Physics(void);

	void initialize();
	void deinitialize();

	void stepSimulation(btScalar timeStep);
	void bebugDrawWorld();
	btRigidBody* addRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	btRigidBody* addStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw = true, const btVector3 & scale = btVector3(1, 1, 1), int collisionFilterGroup = 1, int collisionFilterMask = -1);
	
	static btCollisionShape* CreateStaticCollisionShape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, float scale = 1.f);
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale = btVector3(1, 1, 1));

	static btCollisionShape* CreateStaticCollisionShape(Shape* mesh, const btVector3 & scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(Shape* model, float scale = 1.f);

	static btCollisionShape * CreateStaticCollisionShape(Terrain* terrain, const btVector3& scale);
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(Terrain* terrain, float scale);

	static btCollisionShape* CreateStaticCollisionShape(ObjMesh* mesh, const btVector3& scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(ObjModel * model, const btVector3 & scale);
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(ObjModel * model, float scale = 1.f);	
	static btRigidBody* CreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape);
	static float SweepSphere(const btVector3& from, const btVector3& to, float radius, int collisionFilterGroup = 1, int collisionFilterMask = -1);

	static btTransform BtTransform();
	static btTransform BtTransform(const Vector3f& origin);
	static btTransform BtTransform(const Vector3f& axis, float degrees);
	static btTransform BtTransform(const Vector3f& origin, const Vector3f& axis, float degrees);
	static btVector3 VectorFrom(const Vector3f& vector);

	static Matrix4f MatrixFrom(const btTransform& trans, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static Matrix4f MatrixTransposeFrom(const btTransform& trans, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static Vector3f VectorFrom(const btVector3& vector);
	static Quaternion QuaternionFrom(const btQuaternion& quaternion);

	static btDiscreteDynamicsWorld * GetDynamicsWorld();

private:

	btRigidBody * createRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape);
	btCollisionDispatcher* m_dispatcher;
	//btBroadphaseInterface* m_overlappingPairCache;
	btBroadphaseInterface* m_broadphase;
	btSequentialImpulseConstraintSolver* m_constraintSolver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	float m_physicsStep;

	static btDiscreteDynamicsWorld *DynamicsWorld;
};