#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "Isystem.h"
#include "progression.hpp"
#include "Event/EventEmitter.h"
#include "Entity/vfx-factory.hpp"

class LifeAndDeathSystem : public ISystem {
public:
	LifeAndDeathSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression);
	void update(float deltatime) override;

private:
	Progression& m_progression;
};
