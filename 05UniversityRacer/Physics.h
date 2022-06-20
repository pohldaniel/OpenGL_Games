// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tom� Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tom� Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David �abata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#ifndef PHYSICS_H
#define PHYSICS_H

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicsUtils.h"
#include "PhysicsCar.h"
#include "ObjModel.h"

#define FIXED_SIMULATION_TIMESTEP 1/120.f  /// pevny simulacni krok
#define MAX_SIMULATION_SUBSTEPS   10       /// maximum simulacnich podkroku

/**
 * Zapouzdruje veskerou praci s fyzikalni simulaci pomoci knihovny Bullet Physics (http://bulletphysics.org/).
 * Cerpano z Bullet 2.79 Physics SDK Manual a Bullet demos, konkretne VehicleDemo.
 * Oboji lze nalezt v archivu http://code.google.com/p/bullet/downloads/detail?name=bullet-2.79-rev2440.zip
 * Dale vyuzity stranky knihovny, konkretne wiki a fora.
 */
class Physics
{
public:
    Physics(void); 
    ~Physics(void);

    void Initialize();
    void Deinitialize();

    void StepSimulation(btScalar timeStep);
    btRigidBody * AddRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape);

    void DebugDrawWorld() { m_dynamicsWorld->debugDrawWorld(); } 

	static btCollisionShape * CreateStaticCollisionShape2(Mesh * mesh, const btVector3 & scale = btVector3(1, 1, 1));
	static std::vector<btCollisionShape *> CreateStaticCollisionShapes2(Model * model, const btVector3 & scale);
	static std::vector<btCollisionShape *> CreateStaticCollisionShapes2(Model * model, float scale = 1.f);	
    void AddStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw = true, const btVector3 & scale = btVector3(1,1,1));
    
    PhysicsCar * GetCar() { return m_car; }
    btDiscreteDynamicsWorld * GetDynamicsWorld() { return m_dynamicsWorld; }    
    void AddCar(const btTransform & trans) { (m_car = new PhysicsCar())->Initialize(m_dynamicsWorld, trans); }    

private:

    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_constraintSolver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    
    btDiscreteDynamicsWorld *m_dynamicsWorld;

    PhysicsCar *m_car;
};

#endif
