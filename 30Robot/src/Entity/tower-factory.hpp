#pragma once

#include <entt/entt.hpp>
#include <Components/Components.h>

#include "factory.hpp"
#include "sprite-factory.hpp"
#include "primitive-factory.hpp"

class TowerFactory : public Factory {
public:
	TowerFactory(entt::DefaultRegistry& registry);
	void init();
	virtual ~TowerFactory();

	std::uint32_t createLaser(float posX, float posY);
	std::uint32_t createSlow(float posX, float posY);

private: 
	std::uint32_t create(float posX, float posY);

private:
	SpriteFactory* m_spriteFactory;
	PrimitiveFactory* m_primitiveFactory;
	cmpt::Sprite m_laserTowerSprite;
	cmpt::Sprite m_slowTowerSprite;
	cmpt::Primitive m_healthBackground;
	cmpt::Primitive m_healthBar;
};
