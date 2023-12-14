#include "mirror-factory.hpp"

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "tags.hpp"
#include "constants.hpp"
#include "components/transform.hpp"
#include "components/hitbox.hpp"
#include "components/sprite-animation.hpp"

#include "components/constrained-rotation.hpp"

#include "locator.hpp"
#include "i-random.hpp"

MirrorFactory::MirrorFactory(entt::DefaultRegistry& registry) : Factory(registry)
{
	
}

void MirrorFactory::init() {
	m_spriteFactory = new SpriteFactory();
	m_mirrorSprite = m_spriteFactory->createAtlas("res/images/spritesheets/tower-mirror-100x100.png", glm::vec2(TILE_SIZE), glm::vec2(100));
}

MirrorFactory::~MirrorFactory() {
	glDeleteTextures(1, &m_mirrorSprite.textureID);
	glDeleteVertexArrays(1, &m_mirrorSprite.vaID);
}

std::uint32_t MirrorFactory::create(float posX, float posY) {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	auto myEntity = m_registry.create();
	m_registry.assign<entityTag::Mirror>(myEntity);
	m_registry.assign<cmpt::Sprite>(myEntity, m_mirrorSprite);
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 0, 0);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY), Z_INDEX_TOWER);
	m_registry.assign<cmpt::Hitbox>(myEntity, MIRROR_RADIUS);
	m_registry.assign<cmpt::ConstrainedRotation>(myEntity, 8);
	m_registry.assign<stateTag::RotateableByMouse>(myEntity);
	m_registry.assign<renderOrderTag::o_Building>(myEntity);
	return myEntity;
}
