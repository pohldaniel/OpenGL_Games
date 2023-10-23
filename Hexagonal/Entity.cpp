#include <iostream>
#include <engine/input/Keyboard.h>
#include "Entity.h"

#include "Prefab.h"
#include "AnimationController.h"
#include "Math.hpp"
#include "Application.h"

Entity::Entity(const Prefab& prefab, const Camera& camera, const float& zoomFactor, const float& focusPointX, const float& focusPointY) 
	: prefab(prefab),
	  camera(camera),
	  zoomFactor(zoomFactor),
	  focusPointX(focusPointX),
	  focusPointY(focusPointY) {
	
	m_minX = m_maxX = m_minY = m_maxY = 0;
	m_isSelected = false;
	m_animationController = std::make_unique< eAnimationController>(*prefab.animationController);
	m_movementPlaner = std::make_unique<eMovementPlanner>(*this, 4.0f);

	//std::cout << "Constructor" << this << std::endl;
}

void Entity::update(float dt) {
	//if (!m_isSelected) return;

	m_movementPlaner->Update();

	const float moveSpeed = 0.8f;
	m_directrion.normalize();
	m_directrion *= moveSpeed;
	Math::isometricToCartesian(m_directrion[0], m_directrion[1], 1.0f, 1.0f);

	//m_velocity = m_directrion;

	Vector2f facingDirection;
	if (!m_velocity.zero()) {
		facingDirection = m_velocity.normalize();
		Math::cartesianToIsometric(facingDirection[0], facingDirection[1], 1.0f, 1.0f);
		facingDirection.normalize();
		m_oldFacingDirection = facingDirection;
	}else {
		facingDirection = m_oldFacingDirection * 0.25f;
	}

	m_animationController->SetFloatParameter(xSpeedParameterHash, facingDirection[0]);
	m_animationController->SetFloatParameter(ySpeedParameterHash, facingDirection[1]);
	m_animationController->SetFloatParameter(magnitudeParameterHash, facingDirection.length());
	m_animationController->Update();

	m_position += m_velocity;


}

void Entity::processInput() {
	if (!m_isSelected) return;

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

void Entity::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button, bool down) {
	float mouseViewX = static_cast<float>(mouseX);
	float mouseViewY = static_cast<float>(Application::Height - mouseY);

	if (button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT) {

		float mouseWorldX = mouseViewX + camera.getPositionX();
		float mouseWorldY = mouseViewY + camera.getPositionY();

		float posX = (prefab.boundingBox.posX + m_position[0]) * zoomFactor + (camera.getPositionX() + focusPointX) * (1.0f - zoomFactor);
		float posY = (prefab.boundingBox.posY + m_position[1]) * zoomFactor + (camera.getPositionY() + focusPointY) * (1.0f - zoomFactor);

		if (mouseWorldX > posX && mouseWorldX < posX + prefab.boundingBox.width  * zoomFactor &&
			mouseWorldY > posY && mouseWorldY < posY + prefab.boundingBox.height * zoomFactor) {

			if (down)
				m_isSelected = !m_isSelected;

		}else {
			m_isSelected = false;
		}
	}

	if (m_isSelected && button == Event::MouseButtonEvent::MouseButton::BUTTON_RIGHT && down) {
		//world with zoom
		float mouseWorldX = mouseViewX - focusPointX + zoomFactor * (camera.getPositionX() + focusPointX);
		float mouseWorldY = mouseViewY - focusPointX + zoomFactor * (camera.getPositionY() + focusPointX);
		m_movementPlaner->AddUserWaypoint(Vector2f(mouseWorldX / zoomFactor, mouseWorldY / zoomFactor));
	}
}

void Entity::updateGridBounds() {

	m_minX = static_cast<int>(std::roundf((0.5f * m_position[0] - m_position[1] + prefab.offset[0] - prefab.bounds[3]) / CELL_WIDTH));
	m_maxX = static_cast<int>(std::roundf((0.5f * m_position[0] - m_position[1] + prefab.offset[0] - prefab.bounds[1]) / CELL_WIDTH));

	m_minY = static_cast<int>(std::roundf((-0.5f * m_position[0] - m_position[1] + prefab.offset[1] - prefab.bounds[2]) / CELL_WIDTH));
	m_maxY = static_cast<int>(std::roundf((-0.5f * m_position[0] - m_position[1] + prefab.offset[1] - prefab.bounds[0]) / CELL_WIDTH));

	m_minY += 1;
	m_maxY += 1;

	//m_minX = Math::Clamp(minX, 0, 128);
	//m_maxX = Math::Clamp(minX, 0, 128);
	//m_minY = Math::Clamp(minX, 0, 128);
	//m_maxY = Math::Clamp(minX, 0, 128);
}

void Entity::setPosition(const Vector2f& position) {
	m_position = position;
}