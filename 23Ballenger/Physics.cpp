#include "Physics.h"
#include "engine/ObjModel.h"
#include "engine/MeshObject/Shape.h"
#include "cTerrain.h"

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

void Physics::stepSimulation(btScalar timeStep){
	int numSimSteps = DynamicsWorld->stepSimulation(timeStep, 1, m_physicsStep);
	//int numSimSteps = m_dynamicsWorld->stepSimulation(timeStep, 4 + 1, m_physicsStep / 4); // timeStep < maxSubSteps * fixedTimeSte
}

btRigidBody * Physics::createRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape) {
	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)  //rigidbody is dynamic if and only if mass is non zero, otherwise static
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	//body->setContactProcessingThreshold(0);
	body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_STATIC_OBJECT);
	return body;
}


btRigidBody* Physics::addRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape, int collisionFilterGroup, int collisionFilterMask){
	btRigidBody* body = createRigidBody(mass, startTransform, shape);
	DynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);

	return body;
}

btRigidBody* Physics::addStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw, const btVector3 & scale, int collisionFilterGroup, int collisionFilterMask){
	btRigidBody *body = nullptr; 
	
	for (unsigned int i = 0; i < collisionShapes.size(); i++){
		btCollisionShape *colShape;

		if (scale != btVector3(1, 1, 1))
			colShape = new btScaledBvhTriangleMeshShape((btBvhTriangleMeshShape*)collisionShapes[i], scale);
		else
			colShape = collisionShapes[i];


		body = addRigidBody(0, trans, colShape, collisionFilterGroup, collisionFilterMask);

		if (!debugDraw)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	}

	return body;
}

void Physics::bebugDrawWorld() {
	DynamicsWorld->debugDrawWorld();
}

btCollisionShape * Physics::CreateStaticCollisionShape(ObjMesh* mesh, const btVector3 & scale) {
	int floatsPerVertex = mesh->getStride();
	int integerPerFace = 3;
	int numberOfTriangles = mesh->getIndexBuffer().size() / integerPerFace;
	int numberOfVertices = mesh->getVertexBuffer().size() / floatsPerVertex;

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(numberOfTriangles, (int*)(&mesh->getIndexBuffer()[0]), integerPerFace * sizeof(int), numberOfVertices, (btScalar*)(&mesh->getVertexBuffer()[0]), floatsPerVertex * sizeof(float));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(ObjModel* model, const btVector3 & scale) {
	std::vector<btCollisionShape *> ret;
	for (unsigned int i = 0; i < model->getMeshes().size(); i++) {

		btCollisionShape* shape = CreateStaticCollisionShape(model->getMeshes()[i], scale);

		if (shape) {

			ret.push_back(shape);
		}
	}
	return ret;
}


std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(ObjModel * model, float scale) {
	return CreateStaticCollisionShapes(model, btVector3(scale, scale, scale));
}

btCollisionShape * Physics::CreateStaticCollisionShape(Shape* mesh, const btVector3& scale) {
	
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(mesh->getNumberOfTriangles(), (int*)(&mesh->getIndexBuffer()[0]), indexStride, mesh->getPositions().size(), (btScalar*)(&mesh->getPositions()[0]), sizeof(Vector3f));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(Shape* model, float scale) {
	std::vector<btCollisionShape *> ret;
	btCollisionShape *shape = CreateStaticCollisionShape(model, btVector3(scale, scale, scale));

	if (shape) {
		ret.push_back(shape);
	}
	return ret;
}


btCollisionShape* Physics::CreateStaticCollisionShape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, const btVector3& scale) {
	int floatsPerVertex = 3 ;
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

	btTransform cameraFrom, cameraTo;
	cameraFrom.setIdentity();
	cameraFrom.setOrigin(from);

	cameraTo.setIdentity();
	cameraTo.setOrigin(to);

	btCollisionWorld::ClosestConvexResultCallback cb(from, to);
	cb.m_collisionFilterGroup = collisionFilterGroup;
	cb.m_collisionFilterMask = collisionFilterMask;

	DynamicsWorld->convexSweepTest(&cameraSphere, cameraFrom, cameraTo, cb);

	return cb.m_closestHitFraction;
}

btCollisionShape * Physics::CreateStaticCollisionShape(cTerrain* mesh, const btVector3& scale) {
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(mesh->getNumberOfTriangles(), (int*)(&mesh->getIndexBuffer()[0]), indexStride, mesh->getPositions().size(), (btScalar*)(&mesh->getPositions()[0]), sizeof(Vector3f));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(cTerrain* model, float scale) {
	std::vector<btCollisionShape *> ret;
	btCollisionShape *shape = CreateStaticCollisionShape(model, btVector3(scale, scale, scale));

	if (shape) {
		ret.push_back(shape);
	}
	return ret;
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
	btTransform ret;
	ret.setIdentity();

	return ret;
}

btTransform Physics::BtTransform(const Vector3f& origin) {
	btTransform ret;
	ret.setIdentity();

	ret.setOrigin(btVector3(origin[0], origin[1], origin[2]));

	return ret;
}

btTransform Physics::BtTransform(const Vector3f& axis, float degrees) {
	btTransform ret;
	ret.setIdentity();
	ret.setRotation(btQuaternion(btVector3(axis[0], axis[1], axis[2]), degrees * PI_ON_180));
	return ret;
}

btTransform Physics::BtTransform(const Vector3f& origin, const Vector3f& axis, float degrees) {
	btTransform ret;
	ret.setIdentity();

	ret.setOrigin(btVector3(origin[0], origin[1], origin[2]));
	ret.setRotation(btQuaternion(btVector3(axis[0], axis[1], axis[2]), degrees * PI_ON_180));
	return ret;
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