#pragma once

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <Event/EventEmitter.h>
#include "ISystem.h"

class MovementSystem : public ISystem {
public:
	MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(float deltatime) override;

private:
	glm::vec2 m_mousePos;
	glm::vec2 m_prevMousePos;
};
