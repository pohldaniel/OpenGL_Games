#include <GL/glew.h>
#include <glm/glm.hpp>
#include <Components/Components.h>
#include <Services/IRandom.h>

#include "TowerFactory.h"
#include "Tags.h"
#include "Constants.h"

TowerFactory::TowerFactory(entt::DefaultRegistry& registry) : Factory(registry)
{
	
}

void TowerFactory::init() {
	m_spriteFactory = new SpriteFactory();
	m_primitiveFactory = new PrimitiveFactory();

	m_laserTowerSprite = m_spriteFactory->createAtlas("res/images/spritesheets/tower-laser-100x100.png", glm::vec2(TOWER_HITBOX_RADIUS * 2), glm::vec2(100));
	m_slowTowerSprite = m_spriteFactory->createSingle("res/images/textures/tower-slow.png", glm::vec2(TOWER_HITBOX_RADIUS * 2));
	m_healthBackground = m_primitiveFactory->createRect(glm::vec4(0, 0, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
	m_healthBar = m_primitiveFactory->createRect(glm::vec4(0, 1, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
}

TowerFactory::~TowerFactory() {
	glDeleteTextures(1, &m_laserTowerSprite.textureID);
	glDeleteVertexArrays(1, &m_laserTowerSprite.vaID);
	glDeleteTextures(1, &m_slowTowerSprite.textureID);
	glDeleteVertexArrays(1, &m_slowTowerSprite.vaID);
}

/* ---------------------- Public methods ----------------- */

std::uint32_t TowerFactory::createLaser(float posX, float posY) {
	auto myEntity = create(posX, posY);

	m_registry.assign<cmpt::Sprite>(myEntity, m_laserTowerSprite);
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<cmpt::ShootLaser>(myEntity);
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 0, 0);
	m_registry.assign<cmpt::ConstrainedRotation>(myEntity, 4);
	m_registry.assign<towerTag::LaserTower>(myEntity);
	return myEntity;
}

std::uint32_t TowerFactory::createSlow(float posX, float posY) {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	auto myEntity = create(posX, posY);

	m_registry.assign<cmpt::Sprite>(myEntity, m_slowTowerSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Targeting>(myEntity, -1, TOWER_ATTACK_RANGE);
	m_registry.assign<cmpt::ShootAt>(myEntity, SLOW_TOWER_TIME_BETWEEN_TWO_SHOTS);
	m_registry.assign<projectileType::Slow>(myEntity);
	m_registry.assign<towerTag::SlowTower>(myEntity);
	return myEntity;
}

/*------------------  Private methods ---------- */

std::uint32_t TowerFactory::create(float posX, float posY) {
	auto myEntity = m_registry.create();
	m_registry.assign<entityTag::Tower>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY), Z_INDEX_TOWER);
	m_registry.assign<cmpt::Hitbox>(myEntity, TOWER_HITBOX_RADIUS);
	m_registry.assign<cmpt::Health>(myEntity, TOWER_HEALTH);
	m_registry.assign<cmpt::HealthBar>(myEntity, glm::vec2(-3.0f, -7.0f), m_healthBackground, m_healthBar);
	m_registry.assign<cmpt::Shake>(myEntity);
	m_registry.assign<renderOrderTag::o_Building>(myEntity);
	return myEntity;
}