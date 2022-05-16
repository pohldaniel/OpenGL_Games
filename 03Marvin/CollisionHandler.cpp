#include "CollisionHandler.h"

CollisionHandler::CollisionHandler(b2World &world) : mWorld(world) {}


void CollisionHandler::BeginContact(b2Contact *contact) {

	Object*  firstNode = reinterpret_cast<Object*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Object*  secondNode = reinterpret_cast<Object*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	
	if (!firstNode || !secondNode) return;

	std::pair<Object*, Object*> collisionPair(firstNode, secondNode);
	if (matchesCategories(collisionPair, Category::Type::Player, Category::Type::Exit)) {
		//mWorld.requestCompletion();
		std::cout << "Exit" << std::endl;
	}else if (matchesCategories(collisionPair, Category::Type::Player, Category::Type::Seeker)) {
		std::cout << "Seeker" << std::endl;
	}else if (matchesCategories(collisionPair, Category::Type::Player, Category::Type::Gem)) {
		std::cout << "Gem" << std::endl;
	}
}

void CollisionHandler::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
	//std::cout << "###########" << std::endl;
}

void CollisionHandler::EndContact(b2Contact *contact) {
	//std::cout << "###########" << std::endl;
}


bool CollisionHandler::matchesCategories(std::pair<Object*, Object*> &nodes, unsigned int type1, unsigned int type2) {

	unsigned int cat1 = nodes.first->getCategory();
	unsigned int cat2 = nodes.second->getCategory();

	if ((cat1 & type1) && (cat2 & type2))
		return true;
	else if ((cat1 & type2) && (cat2 & type1)) {
		std::swap(nodes.first, nodes.second);
		return true;
	}
	return false;
}