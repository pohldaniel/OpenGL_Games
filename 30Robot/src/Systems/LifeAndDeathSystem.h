#pragma once
#include <memory>
#include <entt/entt.hpp>
#include <Systems/ISystem.h>
#include <EntityFactories/VfxFactory.h>
#include <Event/EventEmitter.h>

#include "Progression.h"

class LifeAndDeathSystem : public ISystem {
public:
	LifeAndDeathSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression);
	void update(float deltatime) override;

private:
	Progression& m_progression;
};
