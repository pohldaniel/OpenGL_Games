#include "Physics.h"
#include "engine/ObjModel.h"
#include "engine/MeshObject/Shape.h"


void Physics::PreTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	static_cast<Physics*>(world->getWorldUserInfo())->preStep(timeStep);
}

void Physics::PostTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	static_cast<Physics*>(world->getWorldUserInfo())->postStep(timeStep);
}

btDiscreteDynamicsWorld* Physics::DynamicsWorld;

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
}

void Physics::deinitialize(){
	// remove the rigidbodies from the dynamics world and delete them
	for (int i = DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--){
		btCollisionObject* obj = DynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);

		if (body && body->getMotionState())
			delete body->getMotionState();

		if (body && body->getCollisionShape())
			delete body->getCollisionShape();

		DynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	delete DynamicsWorld;
	delete m_constraintSolver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

btBroadphaseInterface* Physics::getBroadphase() {
	return m_broadphase;
}

void Physics::removeCollisionObject(btCollisionObject* obj) {
	btRigidBody* body = btRigidBody::upcast(obj);
	
	if (body && body->getMotionState())
		delete body->getMotionState();

	if (body && body->getCollisionShape())
		delete body->getCollisionShape();

	DynamicsWorld->removeCollisionObject(obj);
	delete obj;
}

void Physics::removeAllCollisionObjects() {

	for (int i = DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
		btCollisionObject* obj = DynamicsWorld->getCollisionObjectArray()[i];		 
		btRigidBody* body = btRigidBody::upcast(obj);

		if (body && body->getMotionState())
			delete body->getMotionState();

		if (body && body->getCollisionShape())
			delete body->getCollisionShape();

		DynamicsWorld->removeCollisionObject(obj);
		delete obj;
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

void Physics::debugDrawWorld() {
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

btCollisionShape* Physics::CreateCollisionShape(Shape* _shape, const btVector3& scale) {
	
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(_shape->getNumberOfTriangles(), (int*)(&_shape->getIndexBuffer()[0]), indexStride, _shape->getPositions().size(), (btScalar*)(&_shape->getPositions()[0]), sizeof(Vector3f));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
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

btRigidBody* Physics::CreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int collisionFlag) {
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass != 0.0f)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(cInfo);

	body->setCollisionFlags(collisionFlag);
	return body;
}

btRigidBody* Physics::AddRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, int collisionFlag) {
	btRigidBody* body = CreateRigidBody(mass, startTransform, shape, collisionFlag);
	DynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);

	return body;
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
	return Quaternion(-quaternion.x(), -quaternion.y(), -quaternion.z(), quaternion.w());
}

btVector3 Physics::VectorFrom(const Vector3f& vector) {
	return btVector3(vector[0], vector[1], vector[2]);
}

btDiscreteDynamicsWorld * Physics::GetDynamicsWorld() {
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