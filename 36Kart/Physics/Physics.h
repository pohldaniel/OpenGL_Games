#pragma once

#include <vector>
#include <unordered_map>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <engine/Vector.h>
#include "DebugDrawer.h"

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

	bool operator !=(const PhysicsRaycastResult& rhs) const {
		return position_ != rhs.position_ || normal_ != rhs.normal_ || distance_ != rhs.distance_ || body_ != rhs.body_;
	}

	Vector3f position_;
	Vector3f normal_;
	float distance_;
	float hitFraction_;
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
		SPHERE = 512,
		DUMMY = 1024,
		SWORD = 2048,
		COL_FORCE_8BIT = 0xFFFF
	};


	Physics(float pysicsStep);
	~Physics(void);

	void initialize();
	void deinitialize();

	void preStep(btScalar timeStep);
	void stepSimulation(btScalar timeStep);
	void postStep(btScalar timeStep);
	btBroadphaseInterface* getBroadphase();
	void HandleCollisions();

	btRigidBody* addStaticModel(std::vector<btCollisionShape*>& collisionShapes, const btTransform& trans, bool debugDraw = true, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f), int collisionFilterGroup = 1, int collisionFilterMask = -1);
	//btRigidBody* addStaticModel(btCollisionShape* collisionShapes, const btTransform& trans, bool debugDraw = true, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f), int collisionFilterGroup = 1, int collisionFilterMask = -1);

	
	static btCollisionShape* CreateCollisionShape(const Shape* shape, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static btCollisionShape* CreateCollisionShape(const std::vector<float>& floatArray, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static btCollisionShape* CreateConvexHullShape(const Shape* shape, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static std::vector<btCollisionShape*> CreateCollisionShapes(Shape* shape, float scale = 1.0f);

	static btCollisionShape* CreateCollisionShape(ObjMesh* mesh, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static btCollisionShape* CreateCollisionShape(ObjMesh* mesh, float scale = 1.0f);
	static std::vector<btCollisionShape*> CreateCollisionShapes(ObjModel* model, const btVector3& scale = btVector3(1.0f, 1.0f, 1.0f));
	static std::vector<btCollisionShape*> CreateCollisionShapes(ObjModel* model, float scale = 1.0f);	

	static btRigidBody* CreateRigidBody(btScalar mass, const btTransform & transform, btCollisionShape* shape, int collisionFlag = btCollisionObject::CF_DYNAMIC_OBJECT, void* userPointer = nullptr);
	static btRigidBody* AddRigidBody(float mass, const btTransform & transform, btCollisionShape* shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, unsigned int collisionFlag = btCollisionObject::CF_DYNAMIC_OBJECT, void* userPointer = nullptr);
	
	static btRigidBody* AddKinematicRigidBody(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = nullptr, bool useMotionState = true);
	static btRigidBody* AddStaticRigidBody(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = nullptr);

	static btCollisionObject* AddKinematicObject(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = nullptr);
	static btCollisionObject* AddKinematicTrigger(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = nullptr);
	static btCollisionObject* AddStaticObject(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = nullptr);
	static btCollisionObject* AddStaticTrigger(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = nullptr);

	static float SweepSphere(const btVector3& from, const btVector3& to, float radius, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	static float RayTest(const btVector3& from, const btVector3& to, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	static void RaycastSingleSegmented(PhysicsRaycastResult& result, const Vector3f& origin, const Vector3f& direction, float maxDistance, float segmentDistance, int collisionFilterGroup = 1, int collisionFilterMask = -1);

	static btTransform BtTransform();
	static btTransform BtTransform(const btVector3& origin);
	static btTransform BtTransform(const btVector3& origin, const btQuaternion& orientation);
	static btTransform BtTransform(const btQuaternion& orientation, const btVector3& origin);

	static btTransform BtTransform(const Vector3f& origin);
	static btTransform BtTransform(const Vector3f& axis, float degrees);
	static btTransform BtTransform(const Vector3f& origin, const Vector3f& axis, float degrees);
	static btVector3 VectorFrom(const Vector3f& vector);
	static btQuaternion QuaternionFrom(const Quaternion& quaternion);

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
	static void DebugDrawWorld();
	static void SetDebugMode(unsigned int mode);
	static void DeleteAllCollisionObjects();
	static void DeleteCollisionObject(btCollisionObject* obj);

	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_broadphase;
	btSequentialImpulseConstraintSolver* m_constraintSolver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	float m_physicsStep;

	static btDiscreteDynamicsWorld* DynamicsWorld;
	static DebugDrawer DebugDrawer;
};