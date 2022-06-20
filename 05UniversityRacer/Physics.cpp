// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tomáš Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tomáš Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David Šabata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#include "Physics.h"


Physics::Physics(void): m_car(NULL)
{
    Initialize();
}

Physics::~Physics(void)
{
    Deinitialize();
}

void Physics::Initialize()
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    
    ///btAxis3Sweep is a good general purpose broadphase
    m_overlappingPairCache = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
    
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    m_constraintSolver = new btSequentialImpulseConstraintSolver;
    
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_constraintSolver, m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

void Physics::Deinitialize()
{
   // remove the rigidbodies from the dynamics world and delete them
   for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
            delete body->getMotionState();
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    if (m_car)
    {
        m_car->Deinitialize();
        delete m_car;
    }

    delete m_dynamicsWorld;
    delete m_constraintSolver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void Physics::StepSimulation(btScalar timeStep)
{
    if (m_car) m_car->Update(timeStep);

    int numSimSteps = m_dynamicsWorld->stepSimulation(timeStep, MAX_SIMULATION_SUBSTEPS, FIXED_SIMULATION_TIMESTEP);

    /*if (!numSimSteps)	std::cout << "Interpolated transforms" << std::endl;
	else  {
		if (numSimSteps > MAX_SIMULATION_SUBSTEPS) // detect dropping frames			
			std::cout << "Dropped " << numSimSteps - MAX_SIMULATION_SUBSTEPS << " simulation steps out of " << numSimSteps << std::endl;
        else
			std::cout << "Simulated " << numSimSteps << " steps" << std::endl;
	}*/
}

btRigidBody * Physics::AddRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape)
{
    btRigidBody* body = PhysicsUtils::CreateRigidBody(mass, startTransform, shape);
    
    m_dynamicsWorld->addRigidBody(body);

    return body;
}


void Physics::AddStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw, const btVector3 & scale)
{
    for (unsigned int i = 0; i < collisionShapes.size(); i++)
    {
        btCollisionShape *colShape;
        
        if (scale != btVector3(1,1,1))
            colShape = new btScaledBvhTriangleMeshShape((btBvhTriangleMeshShape*)collisionShapes[i], scale);
        else
            colShape = collisionShapes[i];

		
        btRigidBody *body = AddRigidBody(0, trans, colShape);

        if (!debugDraw)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }

	/*btTransform transPlane;
	transPlane.setIdentity();

	btCollisionShape *colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

	btVector3 localInertia(0, 0, 0);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(transPlane);

	btRigidBody::btRigidBodyConstructionInfo cInfo(0.0, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(0);

	m_dynamicsWorld->addRigidBody(body);*/

	/*btTransform transPlane;
	transPlane.setIdentity();
	btCollisionShape* btPlane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btVector3 localInertia(0, 0, 0);

	btMotionState* motionPlane = new btDefaultMotionState(transPlane);
	btRigidBody::btRigidBodyConstructionInfo infoPlane(0.0, motionPlane, btPlane);
	btRigidBody* bodyPlane = new btRigidBody(infoPlane);
	bodyPlane->setContactProcessingThreshold(0);
	bodyPlane->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT); 
	m_dynamicsWorld->addRigidBody(bodyPlane);	*/
}

btCollisionShape * Physics::CreateStaticCollisionShape2(Mesh * mesh, const btVector3 & scale) {
	
	btTriangleMesh* tiva = new btTriangleMesh();
	for (int i = 0; i < mesh->m_numberOfTriangles; i++){
		Vector3f v1 = mesh->m_positions[mesh->m_indexBuffer[i*3]];
		Vector3f v2 = mesh->m_positions[mesh->m_indexBuffer[i*3 + 1]];
		Vector3f v3 = mesh->m_positions[mesh->m_indexBuffer[i*3 + 2]];

		btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
		btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
		btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);

		tiva->addTriangle(bv1, bv2, bv3);
	}

	btCollisionShape* shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(btVector3(1.0, 1.0, 1.0));
		
	/*std::vector<std::array<int, 3>> faces;
	//std::vector<int> faces;
	std::vector<Vector3f> vertices;

	for (int i = 0; i < mesh->m_numberOfTriangles; i++) {
		faces.push_back({ mesh->m_indexBuffer[i * 3 + 0],  mesh->m_indexBuffer[i * 3 + 1],  mesh->m_indexBuffer[i * 3 + 2] });
		//faces.push_back(mesh->m_indexBuffer[i * 3 + 0]);
		//faces.push_back(mesh->m_indexBuffer[i * 3 + 1]);
		//faces.push_back(mesh->m_indexBuffer[i * 3 + 2]);
	}

	for (int i = 0; i < mesh->m_positions.size(); i++) {
		vertices.push_back(Vector3f(mesh->m_positions[i][0], mesh->m_positions[i][1], mesh->m_positions[i][2]));
	}

	
	int numTriangles = mesh->m_numberOfTriangles;
	int numVertices = mesh->m_positions.size();
	
	int vertStride = sizeof(Vector3f);
	int indexStride = 3 * sizeof(int);
	
	btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(numTriangles, &faces[0], indexStride,
		numVertices, (btScalar*)(&vertices[0]), vertStride);

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
	shape->setLocalScaling(btVector3(1.0, 1.0, 1.0));*/

	return shape;
}


std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes2(Model * model, const btVector3 & scale){
	std::vector<btCollisionShape *> ret;

	for (unsigned int i = 0; i < model->m_mesh.size(); i++){
		
		btCollisionShape *shape = CreateStaticCollisionShape2(model->m_mesh[i], scale);

		if (shape) {
			
			ret.push_back(shape);
		}
	}

	return ret;
}


std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes2(Model * model, float scale){
	return CreateStaticCollisionShapes2(model, btVector3(scale, scale, scale));
}