#include "CollisionHandler.h"

CollisionHandler::CollisionHandler(b2World &world) : mWorld(world) {}

void CollisionHandler::BeginContact(b2Contact *contact) {


	b2BodyUserData  firstNode = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData  secondNode = contact->GetFixtureB()->GetBody()->GetUserData();

	std::cout << firstNode.pointer << std::endl;
	std::cout << secondNode.pointer << std::endl;
	std::cout << "-------------" << std::endl;
}

void CollisionHandler::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {}

void CollisionHandler::EndContact(b2Contact *contact) {}