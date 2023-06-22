#include "Physics.h"
#include "engine/ObjModel.h"
#include "engine/MeshObject/Shape.h"

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
	//m_broadphase = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	m_broadphase = new btDbvtBroadphase();
	m_constraintSolver = new btSequentialImpulseConstraintSolver();

	DynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration);
	DynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
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
	//delete m_overlappingPairCache;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
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

void Physics::stepSimulation(btScalar timeStep){
	int numSimSteps = DynamicsWorld->stepSimulation(timeStep, 1, m_physicsStep);
	//int numSimSteps = m_dynamicsWorld->stepSimulation(timeStep, 4 + 1, m_physicsStep / 4); // timeStep < maxSubSteps * fixedTimeSte
}

btRigidBody * Physics::createRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int collisionFlag) {
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass != 0.0f)  //rigidbody is dynamic if and only if mass is non zero, otherwise static
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	//body->setContactProcessingThreshold(0);
	body->setCollisionFlags(collisionFlag);
	return body;
}


btRigidBody* Physics::addRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, int collisionFilterGroup, int collisionFilterMask, int collisionFlag){
	btRigidBody* body = createRigidBody(mass, startTransform, shape, collisionFlag);
	DynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);

	return body;
}

btRigidBody* Physics::addStaticModel(std::vector<btCollisionShape *>& collisionShapes, const btTransform& trans, bool debugDraw, const btVector3& scale, int collisionFilterGroup, int collisionFilterMask){
	btRigidBody *body = nullptr; 
	
	for (unsigned int i = 0; i < collisionShapes.size(); i++){
		btCollisionShape *colShape;

		if (scale != btVector3(1.0f, 1.0f, 1.0f))
			colShape = new btScaledBvhTriangleMeshShape((btBvhTriangleMeshShape*)collisionShapes[i], scale);
		else
			colShape = collisionShapes[i];


		body = addRigidBody(0.0f, trans, colShape, collisionFilterGroup, collisionFilterMask, btCollisionObject::CF_STATIC_OBJECT);

		if (!debugDraw)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	}

	return body;
}

void Physics::bebugDrawWorld() {
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

btCollisionShape * Physics::CreateStaticCollisionShape(Shape* _shape, const btVector3& scale) {
	
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(_shape->getNumberOfTriangles(), (int*)(&_shape->getIndexBuffer()[0]), indexStride, _shape->getPositions().size(), (btScalar*)(&_shape->getPositions()[0]), sizeof(Vector3f));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(Shape* _shape, float scale) {
	std::vector<btCollisionShape *> ret;
	btCollisionShape *shape = CreateStaticCollisionShape(_shape, btVector3(scale, scale, scale));

	if (shape) {
		ret.push_back(shape);
	}
	return ret;
}

btCollisionShape* Physics::CreateStaticCollisionShape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale) {
	int floatsPerVertex = 3;
	int integerPerFace = 3;
	int numberOfTriangles = indexBuffer.size() / integerPerFace;
	int numberOfVertices = vertexBuffer.size() / floatsPerVertex;

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(numberOfTriangles, (int*)(&indexBuffer[0]), integerPerFace * sizeof(int), numberOfVertices, (btScalar*)(&vertexBuffer[0]), 3 * sizeof(float));

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

btRigidBody* Physics::CreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape) {

	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass != 0.f)  //rigidbody is dynamic if and only if mass is non zero, otherwise static
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(0);

	return body;
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