#pragma once
#include <iostream>
#include <Box2D\Box2D.h>
#include "GameObject.h"

namespace Category {
	enum Type {
		None = 0, //Unclassified
		Walkable = 1 << 0, //Platforms, tops of boxes, etc
		Player = 1 << 1, //Player
		Enemy = 1 << 2, //Enemy
		GrassBlock = 1 << 3, //GrassBlock entity
		GrassPlatform = 1 << 4, //GrassPlatform entity
		Seeker = 1 << 5, //For following player
		Gem = 1 << 6, //Level gem
		Exit = 1 << 7, //End of level
		Damager = 1 << 8, //Spikes, lava, etc
		SoundEffect = 1 << 9 //Effects
	};
}

class World;

class CollisionHandler : public b2ContactListener {

public:
	CollisionHandler(b2World &world);
	void BeginContact(b2Contact *contact);
	void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
	void EndContact(b2Contact *contact);
	

private:
	bool matchesCategories(std::pair<GameObject*, GameObject*> &nodes, unsigned int type1, unsigned int type2);

private:
	b2World &mWorld;
};