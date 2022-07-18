#pragma once
#include <vector>
#include <btBulletDynamicsCommon.h>

class Mesh;
class Model;
class MeshCube;

class Physics{
public:
	Physics(float pysicsStep);
	~Physics(void);

	void initialize();
	void deinitialize();

	void stepSimulation(btScalar timeStep);
	btRigidBody * addRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape);
	btRigidBody* addStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw = true, const btVector3 & scale = btVector3(1, 1, 1));

	void bebugDrawWorld() { m_dynamicsWorld->debugDrawWorld(); }


	static btCollisionShape * CreateStaticCollisionShape(MeshCube* mesh, const btVector3 & scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape *> CreateStaticCollisionShapes(MeshCube* model, float scale = 1.f);

	static btCollisionShape * CreateStaticCollisionShape(Mesh * mesh, const btVector3 & scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape *> CreateStaticCollisionShapes(Model * model, const btVector3 & scale);
	static std::vector<btCollisionShape *> CreateStaticCollisionShapes(Model * model, float scale = 1.f);
	static btTransform btTransFrom();

	
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