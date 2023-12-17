#include "animation-system.hpp"

#include "constants.hpp"
#include "tags.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "components/transform.hpp"

#include <Event/EventListener.h>

AnimationSystem::AnimationSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: ISystem(registry, emitter), m_vfxFactory(registry)
{
	m_emitter.on<evnt::EnemyDead>([this](const evnt::EnemyDead & event, EventEmitter & emitter) {
		if (event.type == EnemyType::ROBOT) {
			m_vfxFactory.createExplosion(event.position, ShaderType::ENEMY_EXPLOSION);
		}
		if (event.type == EnemyType::KAMIKAZE) {
			m_vfxFactory.createKamikazeExplosion(event.position, KAMIKAZE_EXPLOSION_RADIUS);
		}
	});

	m_emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		m_vfxFactory.createExplosion(event.position, ShaderType::TOWER_EXPLOSION);
	});
}

void AnimationSystem::update(float deltatime) {
    m_registry.view<cmpt::SpriteAnimation, cmpt::Sprite>().each([this,deltatime](auto entity, cmpt::SpriteAnimation& animation, cmpt::Sprite& sprite) {
		animation.age += deltatime;
        if (animation.age < animation.duration) {
            animation.activeTile = animation.startTile + animation.age/animation.duration*(animation.endTile-animation.startTile) ;
        }
		else {
			if (m_registry.has<renderTag::OneTimeAtlas>(entity)) {
				m_registry.destroy(entity);
			}
			else {
				animation.activeTile = animation.startTile;
				animation.age = 0;
			}
        }
    });
}
