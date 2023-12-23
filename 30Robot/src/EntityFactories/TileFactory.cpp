#include <Components/Components.h>
#include "TileFactory.h"

#include "Constants.h"
#include "Tags.h"

TileFactory::TileFactory(entt::DefaultRegistry& registry) : Factory(registry) {
	m_spawn = m_spriteFactory.createSingle("res/images/textures/spawn.png", TILE_SIZE, TILE_SIZE);
	m_arrival = m_spriteFactory.createSingle("res/images/textures/exit.png", TILE_SIZE, TILE_SIZE);
	m_path = m_spriteFactory.createAtlas("res/images/spritesheets/tile-100x100.png", glm::vec2(100), TILE_SIZE, TILE_SIZE);
	m_locked = m_primitiveFactory.createRect(glm::vec4(0.5, 0.5, 0.5, 1), 11.0f, 11.0f);
}

TileFactory::~TileFactory() {
	glDeleteTextures(1, &m_spawn.textureID);
	glDeleteTextures(1, &m_arrival.textureID);
	glDeleteTextures(1, &m_path.textureID);
}

std::uint32_t TileFactory::createSpawn(glm::vec2 position) {
	std::uint32_t myEntity = create(position);
	m_registry.assign<cmpt::Sprite>(myEntity, m_spawn);
	m_registry.assign<renderTag::Single>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createArrival(glm::vec2 position) {
	std::uint32_t myEntity = create(position);
	m_registry.assign<cmpt::Sprite>(myEntity, m_arrival);
	m_registry.assign<renderTag::Single>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createPath(glm::vec2 position) {
	std::uint32_t myEntity = create(position);
	m_registry.assign<cmpt::Sprite>(myEntity, m_path);
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0u, 0u, 0.0f);
	m_registry.assign<tileTag::Path>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createConstructible(glm::vec2 position) {
	std::uint32_t myEntity = create(position);
	m_registry.assign<tileTag::Constructible>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createLocked(glm::vec2 position) {
	std::uint32_t myEntity = create(position);
	m_registry.assign<cmpt::Primitive>(myEntity, m_locked);
	return myEntity;
}

/* ----------- PRIVATE METHODS ----------- */

std::uint32_t TileFactory::create(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Transform>(myEntity, position, Z_INDEX_MAP);
	m_registry.assign<entityTag::Tile>(myEntity);
	m_registry.assign<renderOrderTag::o_Tile>(myEntity);
	return myEntity;
}
