#pragma once
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "engine/Vector.h"

#include "turso3d/Math/Ray.h"


#define MAX_SIMULATION_SUBSTEPS   0

class ObjMesh;
class ObjModel;
class Shape;

class btFilteredVehicleRaycaster : public btVehicleRaycaster {
	btDynamicsWorld* m_dynamicsWorld;

public:

	btFilteredVehicleRaycaster(btDynamicsWorld* world, int collisionFilterGroup, int collisionFilterMask) : m_dynamicsWorld(world) {
		m_collisionFilterGroup = collisionFilterGroup;
		m_collisionFilterMask = collisionFilterMask;
	}

	virtual void* castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result) override;

private:
	int m_collisionFilterGroup = -1;
	int m_collisionFilterMask = -1;
};

struct PhysicsRaycastResult {
	/// Construct with defaults.
	PhysicsRaycastResult() : body_(0) {
	}

	/// Test for inequality, added to prevent GCC from complaining.
	bool operator !=(const PhysicsRaycastResult& rhs) const {
		return position_ != rhs.position_ || normal_ != rhs.normal_ || distance_ != rhs.distance_ || body_ != rhs.body_;
	}

	/// Hit worldspace position.
	Vector3 position_;
	/// Hit worldspace normal.
	Vector3 normal_;
	/// Hit distance from ray origin.
	float distance_;
	/// Hit fraction.
	float hitFraction_;
	/// Rigid body that was hit.
	btRigidBody* body_;
};


class Physics{

public:

	enum collisiontypes {
		FLOOR = 1,
		RENDERABLE_OBJECT = 2,
		RAY = 4,
		PICKABLE_OBJECT = 8,
		CAMERA = 16,
		CHARACTER = 32,
		TRIGGER = 64,
		CAR = 128,
		MOUSEPICKER = 256,
		COL_FORCE_8BIT = 0xFF
	};


	Physics(float pysicsStep);
	~Physics(void);

	void initialize();
	void deinitialize();
	void removeAllCollisionObjects();
	void removeCollisionObject(btCollisionObject* obj);

	void preStep(btScalar timeStep);
	void stepSimulation(btScalar timeStep);
	void postStep(btScalar timeStep);
	void debugDrawWorld();
	btBroadphaseInterface* getBroadphase();

	btRigidBody* addStaticModel(std::vector<btCollisionShape*>& collisionShapes, const btTransform& trans, bool debugDraw = true, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f), int collisionFilterGroup = 1, int collisionFilterMask = -1);
	//btRigidBody* addStaticModel(btCollisionShape* collisionShapes, const btTransform& trans, bool debugDraw = true, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f), int collisionFilterGroup = 1, int collisionFilterMask = -1);

	
	static btCollisionShape* CreateCollisionShape(Shape* shape, const btVector3 & scale = btVector3(1.0f, 1.0f, 1.0f));
	static std::vector<btCollisionShape*> CreateCollisionShapes(Shape* shape, float scale = 1.0f);

	static btCollisionShape* CreateCollisionShape(ObjMesh* mesh, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static btCollisionShape* CreateCollisionShape(ObjMesh* mesh, float scale = 1.0f);
	static std::vector<btCollisionShape*> CreateCollisionShapes(ObjModel* model, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static std::vector<btCollisionShape*> CreateCollisionShapes(ObjModel* model, float scale = 1.0f);	

	static btRigidBody* CreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape, int collisionFlag = 0);
	static btRigidBody* AddRigidBody(float mass, const btTransform & transform, btCollisionShape * shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, int collisionFlag = btCollisionObject::CF_DYNAMIC_OBJECT);


	static float SweepSphere(const btVector3& from, const btVector3& to, float radius, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	static float RayTest(const btVector3& from, const btVector3& to, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	static void RaycastSingleSegmented(PhysicsRaycastResult& result, const Ray& ray, float maxDistance, float segmentDistance, int collisionFilterGroup = 1, int collisionFilterMask = -1);

	static btTransform BtTransform();
	static btTransform BtTransform(const btVector3& origin);
	static btTransform BtTransform(const btVector3& origin, const btQuaternion& orientation);

	static btTransform BtTransform(const Vector3f& origin);
	static btTransform BtTransform(const Vector3f& axis, float degrees);
	static btTransform BtTransform(const Vector3f& origin, const Vector3f& axis, float degrees);
	static btVector3 VectorFrom(const Vector3f& vector);

	static Matrix4f MatrixFrom(const btTransform& trans, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static Matrix4f MatrixTransposeFrom(const btTransform& trans, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static Vector3f VectorFrom(const btVector3& vector);
	static Quaternion QuaternionFrom(const btQuaternion& quaternion);

	static btDiscreteDynamicsWorld* GetDynamicsWorld();

	static btCollisionShape* CreateStaticCollisionShape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, float scale = 1.0f);
	static std::vector<btCollisionShape*> CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));

	static void PreTickCallback(btDynamicsWorld* world, btScalar timeStep);
	static void PostTickCallback(btDynamicsWorld* world, btScalar timeStep);

	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_broadphase;
	btSequentialImpulseConstraintSolver* m_constraintSolver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	float m_physicsStep;

	static btDiscreteDynamicsWorld *DynamicsWorld;
};