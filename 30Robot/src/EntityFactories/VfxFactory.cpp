#include <GL/glew.h>
#include <Components/Components.h>

#include "VfxFactory.h"
#include "Tags.h"
#include "Constants.h"
#include "Globals.h"

VFXFactory::VFXFactory(entt::DefaultRegistry& registry)
: Factory(registry)
{
	m_towerExplosionSprite = m_spriteFactory.createAtlas("res/images/spritesheets/explosion0-100x100.png", glm::vec2(30.0f), glm::vec2(100, 100), ShaderType::TOWER_EXPLOSION, &Globals::shapeManager.get("_quad"), 30.0f, 30.0f);
	m_enemyExplosionSprite = m_spriteFactory.createAtlas("res/images/spritesheets/explosion0-100x100.png", glm::vec2(30.0f), glm::vec2(100, 100), ShaderType::ENEMY_EXPLOSION, &Globals::shapeManager.get("_quad"), 30.0f, 30.0f);
	m_laserParticleSprite = m_spriteFactory.createSingle("res/images/textures/etincelle.png", glm::vec2(5.0f), &Globals::shapeManager.get("_quad"), 5.0f, 5.0f);
}

VFXFactory::~VFXFactory() {
	glDeleteTextures(1, &m_towerExplosionSprite.textureID);
	glDeleteVertexArrays(1, &m_towerExplosionSprite.vaID);

	glDeleteTextures(1, &m_enemyExplosionSprite.textureID);
	glDeleteVertexArrays(1, &m_enemyExplosionSprite.vaID);
}

void VFXFactory::createExplosion(glm::vec2 pos, ShaderType type = ShaderType::ENEMY_EXPLOSION ) {
	auto myEntity = m_registry.create();
	if (type == ShaderType::ENEMY_EXPLOSION) {
		m_registry.assign<cmpt::Sprite>(myEntity, m_enemyExplosionSprite);
	}
	else {
		m_registry.assign<cmpt::Sprite>(myEntity, m_towerExplosionSprite);
	}
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<renderTag::OneTimeAtlas>(myEntity);
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 99, 2);
	m_registry.assign<cmpt::Transform>(myEntity, pos, Z_INDEX_VISUAL_EFFECTS);
	m_registry.assign<renderOrderTag::o_VFX>(myEntity);
}

void VFXFactory::createKamikazeExplosion(glm::vec2 pos, float maxRadius) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Transform>(myEntity, pos, Z_INDEX_VISUAL_EFFECTS);
	m_registry.assign<renderOrderTag::o_VFX>(myEntity);
	m_registry.assign<cmpt::GrowingCircle>(myEntity,KAMIKAZE_EXPLOSION_GROWTH_SPEED, maxRadius);
	m_registry.assign<cmpt::Age>(myEntity, maxRadius/KAMIKAZE_EXPLOSION_GROWTH_SPEED);
	//Create the association between each entity that has health and the explosion
	//It will be removed when the explosion hits and damages the entity (allows the keep track of who has already been damaged and who has not)
	m_registry.view<cmpt::Health>().each([this,myEntity](auto entity, auto) {
		this->m_createEntityExplosionAssociation(entity, myEntity);
	});
}

void VFXFactory::m_createEntityExplosionAssociation(std::uint32_t entity, std::uint32_t explosion) {
	std::uint32_t association = m_registry.create();
	m_registry.assign<cmpt::EntityExplosionAssociation>(association, entity, explosion);
}

void VFXFactory::createLaserParticle(glm::vec2 pos, float dirAgl) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_laserParticleSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, pos, Z_INDEX_VISUAL_EFFECTS);
	m_registry.assign<cmpt::Direction>(myEntity, dirAgl);
	m_registry.assign<cmpt::Velocity>(myEntity, 60.0f);
	m_registry.assign<cmpt::Animated>(myEntity, 0.1f,true);
	//m_registry.assign<cmpt::AnimationAlpha>(myEntity, false);
	m_registry.assign<renderOrderTag::o_VFX>(myEntity);
}