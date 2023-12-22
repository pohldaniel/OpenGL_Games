#include <Components/Components.h>
#include <Services/IRandom.h>
#include <Level/Level.h>

#include "EnemyFactory.h"
#include "Tags.h"
#include "Constants.h"

EnemyFactory::EnemyFactory(entt::DefaultRegistry& registry, Level& level): Factory(registry), m_level(level){
	m_robotSprite = m_spriteFactory.createAtlas("res/images/spritesheets/enemy-small-robot-77x117.png", glm::vec2(77, 117), TILE_SIZE * 0.66, TILE_SIZE);
	m_droneSprite = m_spriteFactory.createSingle("res/images/textures/drone-no-eye.png", 13.0f, 13.0f);
	m_droneEyeSprite = m_spriteFactory.createSingle("res/images/textures/drone-eye.png", 13.0f, 13.0f);
	m_healthBackground = m_primitiveFactory.createRect(glm::vec4(0, 0, 0, 1), 6.0f, 1.0f, 0.5f, 0.5f);
	m_healthBar = m_primitiveFactory.createRect(glm::vec4(0, 1, 0, 1), 6.0f, 1.0f, 0.5f, 0.5f);
}

EnemyFactory::~EnemyFactory() {
	glDeleteTextures(1, &m_droneSprite.textureID);
	glDeleteTextures(1, &m_droneEyeSprite.textureID);
	glDeleteTextures(1, &m_robotSprite.textureID);
}

void EnemyFactory::createRobot() {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	std::uint32_t entity = create();
	m_registry.assign<enemyTag::Robot>(entity);
	m_registry.assign<cmpt::Health>(entity, ENEMY_DRONE_HEALTH + randomService.random(-ENEMY_HEALTH_RANDOM_VARIATION, ENEMY_HEALTH_RANDOM_VARIATION));
	m_registry.assign<cmpt::Sprite>(entity, m_robotSprite);
	m_registry.assign<renderTag::Atlas>(entity);
	m_registry.assign<cmpt::SpriteAnimation>(entity, 0, 11, 0.5);
}

void EnemyFactory::createKamikaze() {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	std::uint32_t entity = create();
	m_registry.assign<cmpt::Sprite>(entity, m_droneSprite);
	m_registry.assign<renderTag::Single>(entity);
	m_registry.assign<enemyTag::Kamikaze>(entity);
	m_registry.assign<cmpt::Health>(entity, ENEMY_KAMIKAZE_HEALTH + randomService.random(-ENEMY_HEALTH_RANDOM_VARIATION, ENEMY_HEALTH_RANDOM_VARIATION));
	m_registry.assign<cmpt::TintColour>(entity, glm::vec4(1, 0, 0, 0.5));

	std::uint32_t eye = m_registry.create();
	m_registry.assign<cmpt::Transform>(eye, glm::vec2(0), Z_INDEX_ENEMY + 1);
	m_registry.assign<cmpt::AttachedTo>(eye, entity);
	m_registry.assign<cmpt::Sprite>(eye, m_droneEyeSprite);
	m_registry.assign<renderTag::Single>(eye);
	m_registry.assign<cmpt::MoveTowardsMouse>(eye, 0.8);
	m_registry.assign<renderOrderTag::o_Enemy2>(eye);
	//m_registry.assign<cmpt::Wiggle>(eye,0.7);
}


/* --------- Private methods ---------- */

std::uint32_t EnemyFactory::create() {
	int startNode = m_level.getGraph()->getStartNodeRandom();

	std::uint32_t myEntity = m_registry.create();
	m_registry.assign<entityTag::Enemy>(myEntity);
	cmpt::Transform transform(m_level.getNodePosition(startNode), Z_INDEX_ENEMY);
	m_registry.assign<cmpt::Transform>(myEntity, transform);
	//m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 25, 5);
	m_registry.assign<cmpt::Pathfinding>(myEntity, &m_level, startNode);
	m_registry.assign<cmpt::HealthBar>(myEntity, glm::vec2(-3.0f, -7.0f), m_healthBackground, m_healthBar);
	m_registry.assign<cmpt::Hitbox>(myEntity, 5.0f);
	m_registry.assign<cmpt::Wiggle>(myEntity, 1);
	m_registry.assign<cmpt::Velocity>(myEntity, ENEMY_VELOCITY);
	m_registry.assign<cmpt::Shake>(myEntity);
	//m_registry.assign<cmpt::TintColour>(myEntity, glm::vec4(randomService.random(), randomService.random(), randomService.random(), 0.5));
	m_registry.assign<cmpt::Animated>(myEntity, 2, false);
	m_registry.assign<cmpt::AnimationScale>(myEntity, true);
	m_registry.assign<cmpt::AnimationDark>(myEntity, true);
	m_registry.assign<cmpt::AnimationAlpha>(myEntity, true);
	m_registry.assign<renderOrderTag::o_Enemy>(myEntity);

	return myEntity;
}
