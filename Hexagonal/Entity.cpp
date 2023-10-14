#include <engine/input/Keyboard.h>
#include "Entity.h"

#include "Prefab.h"
#include "AnimationController.h"
#include "Math.hpp"

Entity::Entity(const Prefab& prefab) : m_prefab(prefab){
	m_position = Vector2f(35.0f, 35.0f);
	Math::cartesianToIsometric(m_position[0], m_position[1]);
	m_position[1] = -m_position[1];
}

void Entity::update(float dt) {

	const float moveSpeed = 0.8f;
	m_directrion.normalize();
	m_directrion *= moveSpeed;
	Math::isometricToCartesian(m_directrion[0], m_directrion[1], 1.0f, 1.0f);

	m_velocity = m_directrion;

	Vector2f facingDirection;
	if (!m_velocity.zero()) {
		facingDirection = m_velocity.normalize();
		Math::cartesianToIsometric(facingDirection[0], facingDirection[1], 1.0f, 1.0f);
		facingDirection.normalize();
		m_oldFacingDirection = facingDirection;
	}
	else {
		facingDirection = m_oldFacingDirection * 0.25f;
	}

	m_prefab.animationController->SetFloatParameter(xSpeedParameterHash, facingDirection[0]);
	m_prefab.animationController->SetFloatParameter(ySpeedParameterHash, facingDirection[1]);
	m_prefab.animationController->SetFloatParameter(magnitudeParameterHash, facingDirection.length());
	m_prefab.animationController->Update();
}

void Entity::processInput() {
	
	Keyboard &keyboard = Keyboard::instance();

	m_directrion = Vector2f();

	if (keyboard.keyDown(Keyboard::KEY_UP)) {
		m_directrion += Vector2f(0.0f, 1.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
		m_directrion += Vector2f(0.0f, -1.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
		m_directrion += Vector2f(-1.0f, 0.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		m_directrion += Vector2f(1.0f, 0.0f);
	}
}