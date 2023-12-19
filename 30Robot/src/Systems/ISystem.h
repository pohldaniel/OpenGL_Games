#pragma once

#include <memory>
#include <entt/entt.hpp>

#include <Event/EventEmitter.h>

class ISystem {
public:
	virtual void update(float deltatime) = 0;

protected:
	ISystem(entt::DefaultRegistry& registry, EventEmitter& emitter) : m_registry(registry), m_emitter(emitter) {}

	entt::DefaultRegistry& m_registry;
	EventEmitter& m_emitter;
};