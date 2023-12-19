#pragma once

#include <Components/Components.h>
#include "IFactory.h"
#include "SpriteFactory.h"

enum class ProjectileType {
	SLOW,
	DAMAGE
};

class ProjectileFactory : public Factory {
public:
	ProjectileFactory(entt::DefaultRegistry& registry);
	virtual ~ProjectileFactory();

	void createSlow(glm::vec2 initialPos, unsigned int targetId);
	void createDamage(glm::vec2 initialPos, unsigned int targetId);

private:
	unsigned int create(glm::vec2 initialPos, unsigned int targetId);
	SpriteFactory m_spriteFactory;
	cmpt::Sprite m_projectileSprite;
};

