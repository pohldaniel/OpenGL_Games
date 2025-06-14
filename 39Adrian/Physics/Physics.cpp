#include "Physics.h"
#include "engine/ObjModel.h"
#include "engine/MeshObject/Shape.h"

static const float M_INFINITY = (float)HUGE_VAL;

void Physics::PreTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	static_cast<Physics*>(world->getWorldUserInfo())->preStep(timeStep);
}

void Physics::PostTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	static_cast<Physics*>(world->getWorldUserInfo())->postStep(timeStep);
}

btDiscreteDynamicsWorld* Physics::DynamicsWorld;
DebugDrawer Physics::DebugDrawer;

Physics::Physics(float physicsStep){
	m_physicsStep = physicsStep;
	initialize();
}

Physics::~Physics(void){
	deinitialize();
}

void Physics::initialize(){
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	///btAxis3Sweep is a good general purpose broadphase
	m_broadphase = new btAxisSweep3(btVector3(-1000.0f, -1000.0f, -1000.0f), btVector3(1000.0f, 1000.0f, 1000.0f));
	//m_broadphase = new btDbvtBroadphase();
	m_constraintSolver = new btSequentialImpulseConstraintSolver();

	DynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration);
	DynamicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));
	DynamicsWorld->getDispatchInfo().m_useContinuous = true;
	DynamicsWorld->getSolverInfo().m_splitImpulse = false;
	DynamicsWorld->setSynchronizeAllMotionStates(true);
	//DynamicsWorld->setInternalTickCallback(Physics::PreTickCallback, static_cast<void*>(this), true);
	//DynamicsWorld->setInternalTickCallback(Physics::PostTickCallback, static_cast<void*>(this), false);
	DynamicsWorld->setDebugDrawer(&DebugDrawer);
}

void Physics::deinitialize(){
	DeleteAllCollisionObjects();
	delete DynamicsWorld;
	delete m_constraintSolver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

btBroadphaseInterface* Physics::getBroadphase() {
	return m_broadphase;
}

void Physics::DeleteCollisionObject(btCollisionObject* obj) {
	btRigidBody* body = btRigidBody::upcast(obj);
	
	if (body && body->getMotionState()) {
		delete body->getMotionState();
		body->setMotionState(nullptr);
	}

	if (body && body->getCollisionShape()) {
		delete body->getCollisionShape();
		body->setCollisionShape(nullptr);
	}

	btGhostObject* ghostObject = btPairCachingGhostObject::upcast(obj);

	if (ghostObject && ghostObject->getCollisionShape()) {
		delete ghostObject->getCollisionShape();
		ghostObject->setCollisionShape(nullptr);
	}

	DynamicsWorld->removeCollisionObject(obj);
	delete obj;
}

void Physics::DeleteAllCollisionObjects() {
	std::vector<btCollisionShape*> shapes;
	for (int i = DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
		btCollisionObject* obj = DynamicsWorld->getCollisionObjectArray()[i];		
		btRigidBody* body = btRigidBody::upcast(obj);
		
		if (body && body->getMotionState()) {
			delete body->getMotionState();
			body->setMotionState(nullptr);
		}

		if (body && body->getCollisionShape() && std::find(shapes.begin(), shapes.end(), body->getCollisionShape()) == shapes.end()) {		
			shapes.push_back(body->getCollisionShape());		
			delete body->getCollisionShape();
			body->setCollisionShape(nullptr);
		}

		btGhostObject* ghostObject = btPairCachingGhostObject::upcast(obj);

		if (ghostObject && ghostObject->getCollisionShape() && std::find(shapes.begin(), shapes.end(), ghostObject->getCollisionShape()) == shapes.end()) {
			shapes.push_back(ghostObject->getCollisionShape());
			delete ghostObject->getCollisionShape();
			ghostObject->setCollisionShape(nullptr);
		}

		DynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	for (int i = DynamicsWorld->getNumConstraints() - 1; i >= 0; i--){
		btTypedConstraint* constraint = DynamicsWorld->getConstraint(i);
		DynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}
}

void Physics::preStep(btScalar timeStep) {

}

void Physics::stepSimulation(btScalar timeStep){
	int numSimSteps = DynamicsWorld->stepSimulation(timeStep, MAX_SIMULATION_SUBSTEPS, m_physicsStep);
}

void Physics::postStep(btScalar timeStep) {

}

btRigidBody* Physics::addStaticModel(std::vector<btCollisionShape *>& collisionShapes, const btTransform& trans, bool debugDraw, const btVector3& scale, int collisionFilterGroup, int collisionFilterMask){
	btRigidBody *body = nullptr; 
	
	for (unsigned int i = 0; i < collisionShapes.size(); i++){
		btCollisionShape *colShape;

		if (scale != btVector3(1.0f, 1.0f, 1.0f))
			colShape = new btScaledBvhTriangleMeshShape((btBvhTriangleMeshShape*)collisionShapes[i], scale);
		else
			colShape = collisionShapes[i];


		body = AddRigidBody(0.0f, trans, colShape, collisionFilterGroup, collisionFilterMask, btCollisionObject::CF_STATIC_OBJECT);

		if (!debugDraw)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	}

	return body;
}

void Physics::DebugDrawWorld() {
	DynamicsWorld->debugDrawWorld();
}

btCollisionShape* Physics::CreateCollisionShape(ObjMesh* mesh, const btVector3& scale) {
	int floatsPerVertex = mesh->getStride();
	int integerPerFace = 3;
	int numberOfTriangles = mesh->getIndexBuffer().size() / integerPerFace;
	int numberOfVertices = mesh->getVertexBuffer().size() / floatsPerVertex;

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(numberOfTriangles, (int*)(&mesh->getIndexBuffer()[0]), integerPerFace * sizeof(int), numberOfVertices, (btScalar*)(&mesh->getVertexBuffer()[0]), floatsPerVertex * sizeof(float));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

btCollisionShape* Physics::CreateCollisionShape(ObjMesh* mesh, float scale) {
	return CreateCollisionShape(mesh, btVector3(scale, scale, scale));
}

std::vector<btCollisionShape *> Physics::CreateCollisionShapes(ObjModel* model, const btVector3& scale) {
	std::vector<btCollisionShape *> ret;
	for (unsigned int i = 0; i < model->getMeshes().size(); i++) {

		btCollisionShape* shape = CreateCollisionShape(model->getMeshes()[i], scale);

		if (shape) {

			ret.push_back(shape);
		}
	}
	return ret;
}


std::vector<btCollisionShape *> Physics::CreateCollisionShapes(ObjModel* model, float scale) {
	return CreateCollisionShapes(model, btVector3(scale, scale, scale));
}

btCollisionShape* Physics::CreateCollisionShape(const Shape* _shape, const btVector3& scale) {	
	int indexStride = 3 * sizeof(int);
	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(_shape->getNumberOfTriangles(), (int*)(&_shape->getIndexBuffer()[0]), indexStride, _shape->getPositions().size(), (btScalar*)(&_shape->getPositions()[0]), sizeof(Vector3f));
	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

btCollisionShape* Physics::CreateCollisionShape(const std::vector<float>& floatArray, const btVector3& scale) {
	btTriangleMesh* mesh = new btTriangleMesh();
	for (size_t i = 0; i < floatArray.size() / 9; i++) {
		btVector3 vertex0(floatArray[i * 9    ], floatArray[i * 9 + 1], floatArray[i * 9 + 2]);
		btVector3 vertex1(floatArray[i * 9 + 3], floatArray[i * 9 + 4], floatArray[i * 9 + 5]);
		btVector3 vertex2(floatArray[i * 9 + 6], floatArray[i * 9 + 7], floatArray[i * 9 + 8]);
		mesh->addTriangle(vertex0, vertex1, vertex2);
	}

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(mesh, true);
	shape->setLocalScaling(scale);

	return shape;
}

btCollisionShape* Physics::CreateConvexHullShape(const Shape* _shape, const btVector3 & scale) {
	btConvexHullShape* convexHull = new btConvexHullShape((btScalar*)(&_shape->getPositions()[0]), _shape->getPositions().size(), sizeof(Vector3f));
	convexHull->setLocalScaling(scale);
	return convexHull;
}

btCollisionShape* Physics::CreateConvexHullShape(const std::vector<Vector3f>& positions, const btVector3& scale) {
	btConvexHullShape* convexHull = new btConvexHullShape((btScalar*)(&positions[0]), positions.size(), sizeof(Vector3f));
	convexHull->setLocalScaling(scale);
	return convexHull;
}

std::vector<btCollisionShape*> Physics::CreateCollisionShapes(Shape* _shape, float scale) {
	std::vector<btCollisionShape *> ret;
	btCollisionShape *shape = CreateCollisionShape(_shape, btVector3(scale, scale, scale));

	if (shape) {
		ret.push_back(shape);
	}
	return ret;
}

btCollisionShape* Physics::CreateStaticCollisionShape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale) {
	int floatsPerVertex = 8;
	int integerPerFace = 3;
	int numberOfTriangles = indexBuffer.size() / integerPerFace;
	int numberOfVertices = vertexBuffer.size() / floatsPerVertex;

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(numberOfTriangles, (int*)(&indexBuffer[0]), integerPerFace * sizeof(int), numberOfVertices, (btScalar*)(&vertexBuffer[0]), floatsPerVertex * sizeof(float));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

std::vector<btCollisionShape*> Physics::CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, float scale) {
	std::vector<btCollisionShape*> ret;
	btCollisionShape* shape = CreateStaticCollisionShape(vertexBuffer, indexBuffer, btVector3(scale, scale, scale));

	if (shape) {
		ret.push_back(shape);
	}
	return ret;
}

std::vector<btCollisionShape*> Physics::CreateStaticCollisionShapes(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale) {
	std::vector<btCollisionShape*> ret;
	btCollisionShape* shape = CreateStaticCollisionShape(vertexBuffer, indexBuffer, scale);

	if (shape) {
		ret.push_back(shape);
	}
	return ret;
}

btRigidBody* Physics::CreateRigidBody(btScalar mass, const btTransform& transform, btCollisionShape* shape, int collisionFlag, void* userPointer) {
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass != 0.0f)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(cInfo);

	body->setCollisionFlags(collisionFlag);
	if (userPointer)
		body->setUserPointer(userPointer);
	return body;
}

btRigidBody* Physics::AddRigidBody(float mass, const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, unsigned int collisionFlag, void* userPointer) {
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass != 0.0f)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(cInfo);
	body->setCollisionFlags(collisionFlag);
	if (userPointer)
		body->setUserPointer(userPointer);

	DynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);
	return body;
}

btRigidBody* Physics::AddKinematicRigidBody(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, void* userPointer, bool useMotionState) {
	if (useMotionState) {
		btDefaultMotionState* motionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo cInfo(0.0f, motionState, shape);
		btRigidBody* body = new btRigidBody(cInfo);
		if (userPointer)
			body->setUserPointer(userPointer);
		body->forceActivationState(DISABLE_DEACTIVATION);
		body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		DynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);
		return body;
	}else {
		btRigidBody* body = new btRigidBody(0.0f, nullptr, shape);
		body->setWorldTransform(transform);

		if (userPointer)
			body->setUserPointer(userPointer);
		body->forceActivationState(DISABLE_DEACTIVATION);
		body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

		DynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);
		return body;
	}
}

btRigidBody* Physics::AddStaticRigidBody(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, void* userPointer) {	

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(0.0f, motionState, shape);
	btRigidBody* body = new btRigidBody(cInfo);
	if (userPointer)
		body->setUserPointer(userPointer);
	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

	DynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);
	return body;
}

btCollisionObject* Physics::AddKinematicObject(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, void* userPointer) {
	btCollisionObject* colObj = new btCollisionObject();
	colObj->setWorldTransform(transform);
	colObj->setCollisionShape(shape);
	colObj->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	if (userPointer)
		colObj->setUserPointer(userPointer);
	colObj->forceActivationState(DISABLE_DEACTIVATION);

	DynamicsWorld->addCollisionObject(colObj, collisionFilterGroup, collisionFilterMask);
	return colObj;
}

btCollisionObject* Physics::AddKinematicTrigger(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, void* userPointer) {
	btCollisionObject* colObj = new btCollisionObject();
	colObj->setWorldTransform(transform);
	colObj->setCollisionShape(shape);
	colObj->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	if (userPointer)
		colObj->setUserPointer(userPointer);
	colObj->forceActivationState(DISABLE_DEACTIVATION);

	DynamicsWorld->addCollisionObject(colObj, collisionFilterGroup, collisionFilterMask);
	return colObj;
}

btCollisionObject* Physics::AddStaticObject(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, void* userPointer) {
	btCollisionObject* colObj = new btCollisionObject();
	colObj->setWorldTransform(transform);
	colObj->setCollisionShape(shape);
	colObj->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	if (userPointer)
		colObj->setUserPointer(userPointer);

	DynamicsWorld->addCollisionObject(colObj, collisionFilterGroup, collisionFilterMask);
	return colObj;
}

btCollisionObject* Physics::AddStaticTrigger(const btTransform& transform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, void* userPointer) {
	btCollisionObject* colObj = new btCollisionObject();
	colObj->setWorldTransform(transform);
	colObj->setCollisionShape(shape);
	colObj->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	if (userPointer)
		colObj->setUserPointer(userPointer);

	DynamicsWorld->addCollisionObject(colObj, collisionFilterGroup, collisionFilterMask);
	return colObj;
}

float Physics::SweepSphere(const btVector3& from, const btVector3& to, float radius, int collisionFilterGroup, int collisionFilterMask) {
	btSphereShape cameraSphere(radius);

	btTransform tFrom, tTo;
	tFrom.setIdentity();
	tFrom.setOrigin(from);

	tTo.setIdentity();
	tTo.setOrigin(to);

	btCollisionWorld::ClosestConvexResultCallback cb(from, to);
	cb.m_collisionFilterGroup = collisionFilterGroup;
	cb.m_collisionFilterMask = collisionFilterMask;

	DynamicsWorld->convexSweepTest(&cameraSphere, tFrom, tTo, cb);

	return cb.m_closestHitFraction;
}

float Physics::RayTest(const btVector3& from, const btVector3& to, int collisionFilterGroup, int collisionFilterMask) {
	
	btCollisionWorld::ClosestRayResultCallback cb(from, to);
	cb.m_collisionFilterGroup = collisionFilterGroup;
	cb.m_collisionFilterMask = collisionFilterMask;
	
	DynamicsWorld->rayTest(from, to, cb);

	return cb.m_closestHitFraction;
}

void Physics::RaycastSingleSegmented(PhysicsRaycastResult& result, const Vector3f& _origin, const Vector3f& _direction, float maxDistance, float segmentDistance, int collisionFilterGroup, int collisionMask) {


	btVector3 start = VectorFrom(_origin);
	btVector3 end;
	btVector3 direction = VectorFrom(_direction);
	float distance;

	for (float remainingDistance = maxDistance; remainingDistance > 0; remainingDistance -= segmentDistance) {
		distance = std::min(remainingDistance, segmentDistance);

		end = start + distance * direction;

		btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
		rayCallback.m_collisionFilterGroup = collisionFilterGroup;
		rayCallback.m_collisionFilterMask = collisionMask;

		DynamicsWorld->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);

		if (rayCallback.hasHit())
		{
			result.position_ = VectorFrom(rayCallback.m_hitPointWorld);
			result.normal_ = VectorFrom(rayCallback.m_hitNormalWorld);
			result.distance_ = (result.position_ - _origin).length();
			result.hitFraction_ = rayCallback.m_closestHitFraction;
			result.body_ = static_cast<btRigidBody*>(rayCallback.m_collisionObject->getUserPointer());
			// No need to cast the rest of the segments
			return;
		}

		// Use the end position as the new start position
		start = end;
	}

	// Didn't hit anything
	result.position_ = Vector3f::ZERO;
	result.normal_ = Vector3f::ZERO;
	result.distance_ = M_INFINITY;
	result.hitFraction_ = 0.0f;
	result.body_ = 0;
}

btTransform Physics::BtTransform() {
	btTransform transform;
	transform.setIdentity();
	return transform;
}

btTransform Physics::BtTransform(const btVector3& origin) {
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(origin);
	return transform;
}

btTransform Physics::BtTransform(const btVector3& origin, const btQuaternion& orientation) {
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(origin);
	transform.setRotation(orientation);
	return transform;
}

btTransform Physics::BtTransform(const btQuaternion& orientation, const btVector3& origin) {
	return btTransform(orientation, origin);
}

btTransform Physics::BtTransform(const Vector3f& origin) {
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(origin[0], origin[1], origin[2]));
	return transform;
}

btTransform Physics::BtTransform(const Vector3f& axis, float degrees) {
	btTransform transform;
	transform.setIdentity();
	transform.setRotation(btQuaternion(btVector3(axis[0], axis[1], axis[2]), degrees * PI_ON_180));
	return transform;
}

btTransform Physics::BtTransform(const Vector3f& origin, const Vector3f& axis, float degrees) {
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(origin[0], origin[1], origin[2]));
	transform.setRotation(btQuaternion(btVector3(axis[0], axis[1], axis[2]), degrees * PI_ON_180));
	return transform;
}

btTransform Physics::BtTransform(const Vector3f& origin, const Quaternion& rotation) {
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(origin[0], origin[1], origin[2]));
	transform.setRotation(btQuaternion(rotation[0], rotation[1], rotation[2], rotation[3]));
	return transform;
}

Matrix4f Physics::MatrixFrom(const btTransform& trans, const btVector3& scale) {
	btMatrix3x3 m = trans.getBasis();
	btVector3 v = trans.getOrigin();

	return Matrix4f(m[0].x() * scale[0], m[1].x() * scale[1], m[2].x() * scale[2], 0.0f,
		m[0].y() * scale[0], m[1].y() * scale[1], m[2].y() * scale[2], 0.0f,
		m[0].z() * scale[0], m[1].z() * scale[1], m[2].z() * scale[2], 0.0f,
		v.x(), v.y(), v.z(), 1.0f);
}

Matrix4f Physics::MatrixTransposeFrom(const btTransform& trans, const btVector3& scale) {
	btMatrix3x3 m = trans.getBasis();
	btVector3 v = trans.getOrigin();

	return Matrix4f(m[0].x() * scale[0], m[0].y() * scale[1], m[0].z() * scale[2], v.x(),
		m[1].x() * scale[0], m[1].y() * scale[1], m[1].z() * scale[2], v.y(),
		m[2].x() * scale[0], m[2].y() * scale[1], m[2].z() * scale[2], v.z(),
		0.0f, 0.0f, 0.0f, 1.0f);
}

Vector3f Physics::VectorFrom(const btVector3& vector) {
	return Vector3f(vector.x(), vector.y(), vector.z());
}

Quaternion Physics::QuaternionFrom(const btQuaternion& quaternion) {
	return Quaternion(quaternion.x(), quaternion.y(), quaternion.z(), quaternion.w());
}

btVector3 Physics::VectorFrom(const Vector3f& vector) {
	return btVector3(vector[0], vector[1], vector[2]);
}

btQuaternion Physics::QuaternionFrom(const Quaternion& quaternion) {
	return btQuaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
}

btDiscreteDynamicsWorld* Physics::GetDynamicsWorld() {
	return DynamicsWorld;
}

void* btFilteredVehicleRaycaster::castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result) {
	//	RayResultCallback& resultCallback;

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);


	rayCallback.m_collisionFilterGroup = m_collisionFilterGroup;
	rayCallback.m_collisionFilterMask = m_collisionFilterMask;

	m_dynamicsWorld->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
		if (body && body->hasContactResponse())
		{
			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
			result.m_hitNormalInWorld.normalize();
			result.m_distFraction = rayCallback.m_closestHitFraction;
			return (void*)body;
		}
	}
	return 0;
}

void Physics::HandleCollisions() {
	int numManifolds = m_dispatcher->getNumManifolds();

	if (numManifolds) {
	
		for (int i = 0; i < numManifolds; ++i){

			btPersistentManifold* contactManifold = m_dispatcher->getManifoldByIndexInternal(i);
			// First check that there are actual contacts, as the manifold exists also when objects are close but not touching
			if (!contactManifold->getNumContacts())
				continue;

			const btCollisionObject* objectA = contactManifold->getBody0();
			const btCollisionObject* objectB = contactManifold->getBody1();

		}
	
	}
}

void Physics::SetDebugMode(unsigned int mode) {
	DebugDrawer.setDebugMode(mode);
}