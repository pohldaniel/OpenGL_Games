#pragma once

#include <entt/entt.hpp>
#include <Components/Components.h>

#include "factory.hpp"
#include "sprite-factory.hpp"


class MirrorFactory : public Factory {
public:
	MirrorFactory(entt::DefaultRegistry& registry);
	void MirrorFactory::init();
	virtual ~MirrorFactory();

	std::uint32_t create(float posX, float posY);

private:
	SpriteFactory* m_spriteFactory;
	cmpt::Sprite m_mirrorSprite;
};
