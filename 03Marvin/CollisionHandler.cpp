#include "CollisionHandler.h"
#include "Level.h"

CollisionHandler::CollisionHandler(b2World &world, Level &level) : mWorld(world), m_level(level) {}


void CollisionHandler::BeginContact(b2Contact *contact) {

	Object*  firstNode = reinterpret_cast<Object*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Object*  secondNode = reinterpret_cast<Object*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	
	if (!firstNode || !secondNode) return;

	std::pair<Object*, Object*> collisionPair(firstNode, secondNode);
	if (matchesCategories(collisionPair, Category::Type::Player, Category::Type::Exit)) {		
		//m_level.reset();
		if (firstNode->getCategory() & Category::Type::Player) {
			dynamic_cast<Player*>(firstNode)->setFade(true);
		}

		if (secondNode->getCategory() & Category::Type::Player) {			
			dynamic_cast<Player*>(secondNode)->setFade(true);
		}

	}else if (matchesCategories(collisionPair, Category::Type::Player, Category::Type::Seeker)) {
		m_level.reset();
		if (firstNode->getCategory() & Category::Type::Player) {
			dynamic_cast<Player*>(firstNode)->setPosition(m_level.m_playerPosition);
		}

		if (secondNode->getCategory() & Category::Type::Player) {
			dynamic_cast<Player*>(secondNode)->setPosition(m_level.m_playerPosition);
		}

	}else if (matchesCategories(collisionPair, Category::Type::Player, Category::Type::Enemy)) {
		m_level.reset();
		if (firstNode->getCategory() & Category::Type::Player) {
			dynamic_cast<Player*>(firstNode)->setPosition(m_level.m_playerPosition);
		}

		if (secondNode->getCategory() & Category::Type::Player) {
			dynamic_cast<Player*>(secondNode)->setPosition(m_level.m_playerPosition);
		}

	}else if (matchesCategories(collisionPair, Category::Type::Player, Category::Type::Gem)) {
		
		b2Filter filter;
		if (firstNode->getCategory() & Category::Type::Gem) {
			dynamic_cast<RenderableObject*>(firstNode)->setDisabled(true);
			filter = contact->GetFixtureA()->GetFilterData();
			filter.categoryBits = filter.categoryBits | Category::Type::None;
			contact->GetFixtureA()->SetFilterData(filter);			
		}
		if (secondNode->getCategory() & Category::Type::Gem) {
			dynamic_cast<RenderableObject*>(secondNode)->setDisabled(true);
			filter = contact->GetFixtureB()->GetFilterData();			
			filter.categoryBits = filter.categoryBits | Category::Type::None;
			contact->GetFixtureB()->SetFilterData(filter);				
		}
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