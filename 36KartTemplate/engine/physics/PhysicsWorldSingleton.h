#ifndef PHYSICSWORLDSINGLETON_CLASS_H
#define PHYSICSWORLDSINGLETON_CLASS_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>

class PhysicsWorldSingleton {
private:
    PhysicsWorldSingleton();
    static PhysicsWorldSingleton* instance;

    btBroadphaseInterface* broadphase; //Broad Phase when checking for Collisions

    btDefaultCollisionConfiguration* collisionConfiguration; // Coupled with "dispatcher", the config is default for now...

    btCollisionDispatcher* dispatcher; //Setting a Collision Algorithm 

    btSequentialImpulseConstraintSolver* solver;

public:
    btDiscreteDynamicsWorld* dynamicsWorld;

    static PhysicsWorldSingleton* getInstance();
};

#endif