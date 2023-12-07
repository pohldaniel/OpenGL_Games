#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "Isystem.h"
#include "Event/EventEmitter.h"

#include "Entity/vfx-factory.hpp"

class AnimationSystem : public ISystem {
public:
    AnimationSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
    void update(float deltatime) override;

private:
	VFXFactory m_vfxFactory;
};
