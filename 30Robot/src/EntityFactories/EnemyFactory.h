#pragma once

#include <entt/entt.hpp>
#include <Components/Components.h>
#include <Level/Level.h>

#include "IFactory.h"
#include "SpriteFactory.h"
#include "PrimitiveFactory.h"
#include "ProjectileFactory.h"

class EnemyFactory : public Factory {
public:
	EnemyFactory(entt::DefaultRegistry& registry, Level& level);
	virtual ~EnemyFactory();

	void createRobot();
	void createKamikaze();

private:
	std::uint32_t create();

private:
	SpriteFactory m_spriteFactory;
	PrimitiveFactory m_primitiveFactory;
	cmpt::Sprite m_droneSprite;
	cmpt::Sprite m_droneEyeSprite;
	cmpt::Sprite m_robotSprite;
	cmpt::Primitive m_healthBackground;
	cmpt::Primitive m_healthBar;
	Level& m_level;
};

