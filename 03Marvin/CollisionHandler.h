#pragma once
#include <iostream>
#include <Box2D\Box2D.h>
#include "Object.h"
#include "RenderableObject.h"
#include "Entity.h"
#include "Player.h"

class World;
class Level;
class CollisionHandler : public b2ContactListener {

public:

	CollisionHandler(b2World &world, Level &level);
	void BeginContact(b2Contact *contact);
	void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
	void EndContact(b2Contact *contact);
	

private:
	bool matchesCategories(std::pair<Object*, Object*> &nodes, unsigned int type1, unsigned int type2);

private:
	b2World &mWorld;

	Level &m_level;
};