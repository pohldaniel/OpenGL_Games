#pragma once
#include "Physics.h"
#include "BspConverter.h"

class BspToBulletConverter : public BspConverter {
	
public:

	BspToBulletConverter()  { }

	virtual void	addConvexVerticesCollider(btAlignedObjectArray<btVector3>& vertices, bool isEntity, const btVector3& entityTargetLocation) {
		///perhaps we can do something special with entities (isEntity)
		///like adding a collision Triggering (as example)

		if (vertices.size() > 0) {
			float mass = 0.f;
			btTransform startTransform;
			//can use a shift
			startTransform.setIdentity();
			startTransform.setOrigin(btVector3(0, -10.0f, 0.0f));
			//this create an internal copy of the vertices
			for (int i = 0; i < vertices.size(); i++){
				vertices[i] *= btScalar(0.5);
				float t = vertices[i].getZ() * btScalar(0.75);
				vertices[i].setZ(-vertices[i].getY());
				vertices[i].setY(t);
			}

			Physics::AddRigidBody(mass, startTransform, new btConvexHullShape(&(vertices[0].getX()), vertices.size()));
		}
	}
};