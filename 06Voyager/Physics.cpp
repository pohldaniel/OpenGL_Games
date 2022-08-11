#include "Physics.h"
#include "engine/ObjModel.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/MeshObject/MeshQuad.h"
#include "Terrain.h"
#include "PhysicsCar.h"

void* btFilteredVehicleRaycaster::castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result){
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
	//m_overlappingPairCache = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	m_broadphase = new btDbvtBroadphase();
	m_constraintSolver = new btSequentialImpulseConstraintSolver();

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

void Physics::deinitialize(){
	// remove the rigidbodies from the dynamics world and delete them
	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--){
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	delete m_dynamicsWorld;
	delete m_constraintSolver;
	//delete m_overlappingPairCache;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

void Physics::stepSimulation(btScalar timeStep){
	int numSimSteps = m_dynamicsWorld->stepSimulation(timeStep, 1, m_physicsStep);
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
	m_dynamicsWorld->addRigidBody(body, collisionFilterGroup, collisionFilterMask);

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


btCollisionShape * Physics::CreateStaticCollisionShape(Mesh * mesh, const btVector3 & scale) {
	int indexStride = 3 * sizeof(int);
	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(mesh->m_numberOfTriangles, (int*)(&mesh->m_indexBuffer[0]), indexStride,
		mesh->m_positions.size(), (btScalar*)(&mesh->m_vertexBuffer[0]), 14 * sizeof(float));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}


std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(Model* model, const btVector3 & scale) {
	std::vector<btCollisionShape *> ret;
	for (unsigned int i = 0; i < model->getMeshes().size(); i++) {

		btCollisionShape* shape = CreateStaticCollisionShape(model->getMeshes()[i], scale);

		if (shape) {

			ret.push_back(shape);
		}
	}
	return ret;
}


std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(Model * model, float scale) {
	return CreateStaticCollisionShapes(model, btVector3(scale, scale, scale));
}

btCollisionShape * Physics::CreateStaticCollisionShape(MeshCube * mesh, const btVector3& scale) {
	
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(mesh->getNumberOfTriangles(), (int*)(&mesh->m_indexBuffer[0]), indexStride, mesh->m_positions.size(), (btScalar*)(&mesh->m_positions[0]), sizeof(Vector3f));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}

std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(MeshCube* model, float scale) {
	std::vector<btCollisionShape *> ret;
	btCollisionShape *shape = CreateStaticCollisionShape(model, btVector3(scale, scale, scale));

	if (shape) {
		ret.push_back(shape);
	}
	return ret;
}

btCollisionShape* Physics::CreateStaticCollisionShape(MeshQuad* mesh, const btVector3& scale) {
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(mesh->getNumberOfTriangles(), (int*)(&mesh->m_indexBuffer[0]), indexStride, mesh->m_positions.size(), (btScalar*)(&mesh->m_positions[0]), sizeof(Vector3f));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
} 

std::vector<btCollisionShape*> Physics::CreateStaticCollisionShapes(MeshQuad* model, float scale) {
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

Matrix4f Physics::MatrixFrom(const btTransform& trans, const btVector3& scale){
	btMatrix3x3 m = trans.getBasis();
	btVector3 v = trans.getOrigin();

	return Matrix4f(m[0].x() * scale[0], m[0].y() * scale[1], m[0].z() * scale[2], v.x(),
					m[1].x() * scale[0], m[1].y() * scale[1], m[1].z() * scale[2], v.y(),
					m[2].x() * scale[0], m[2].y() * scale[1], m[2].z() * scale[2], v.z(),
					0.0f, 0.0f, 0.0f, 1.0f);
}


btCollisionShape * Physics::CreateStaticCollisionShape(Terrain* mesh, const btVector3& scale) {
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(mesh->getNumberOfTriangles(), (int*)(&mesh->m_indexBuffer[0]), indexStride, mesh->m_positions.size(), (btScalar*)(&mesh->m_positions[0]), sizeof(Vector3f));

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(scale);

	return shape;
}


std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(Terrain* model, float scale) {
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