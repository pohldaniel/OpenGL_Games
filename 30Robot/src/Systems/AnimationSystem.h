#pragma once
#include <memory>
#include <entt/entt.hpp>
#include <Systems/ISystem.h>
#include <Event/EventEmitter.h>
#include <EntityFactories/VfxFactory.h>

class AnimationSystem : public ISystem {
public:
    AnimationSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
    void update(float deltatime) override;

private:
	VFXFactory m_vfxFactory;
};
