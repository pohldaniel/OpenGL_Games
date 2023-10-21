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
	  focusPointY(focusPointY)
{
	m_position = Vector2f(35.0f, 35.0f);
	Math::cartesianToIsometric(m_position[0], m_position[1]);
	m_position[1] = -m_position[1];

	m_minX = m_maxX = m_minY = m_maxY = 0;
	m_isSelected = false;
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

	prefab.animationController->SetFloatParameter(xSpeedParameterHash, facingDirection[0]);
	prefab.animationController->SetFloatParameter(ySpeedParameterHash, facingDirection[1]);
	prefab.animationController->SetFloatParameter(magnitudeParameterHash, facingDirection.length());
	prefab.animationController->Update();
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

void Entity::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {
	float mouseViewX = static_cast<float>(mouseX);
	float mouseViewY = static_cast<float>(Application::Height - mouseY);

	float mouseWorldX = mouseViewX + zoomFactor * (camera.getPositionX() + focusPointX) - focusPointX;
	float mouseWorldY = mouseViewY + zoomFactor * (camera.getPositionY() + focusPointY) - focusPointY;

	if (m_isSelected && button == Event::MouseButtonEvent::MouseButton::BUTTON_RIGHT) {
		m_isSelected = false;
		return;
	}

	if (mouseWorldX > m_position[0] + prefab.boundingBox.posX && mouseWorldX < m_position[0] + prefab.boundingBox.posX + prefab.boundingBox.width &&
		mouseWorldY > m_position[1] + prefab.boundingBox.posY && mouseWorldY < m_position[1] + prefab.boundingBox.posY + prefab.boundingBox.height) {
		m_isSelected = button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT;
	}else {
		//m_isSelected = false;
	}
}

void Entity::updateGridBounds() {
	int row, col;
	float posX = m_position[0], posY = m_position[1];
	Math::isometricToCartesian(posX, posY, row, col);

	m_minX = static_cast<int>(std::roundf((prefab.offset[0] - prefab.bounds[3]) / CELL_WIDTH));
	m_maxX = static_cast<int>(std::roundf((prefab.offset[0] - prefab.bounds[1]) / CELL_HEIGHT));

	m_minY = static_cast<int>(std::roundf((prefab.offset[1] - prefab.bounds[2]) / CELL_WIDTH));
	m_maxY = static_cast<int>(std::roundf((prefab.offset[1] - prefab.bounds[0]) / CELL_HEIGHT));

	m_minX += row;
	m_maxX += row;
	m_minY += col;
	m_maxY += col;

	//m_minX = Math::Clamp(minX, 0, 128);
	//m_maxX = Math::Clamp(minX, 0, 128);
	//m_minY = Math::Clamp(minX, 0, 128);
	//m_maxY = Math::Clamp(minX, 0, 128);
}

void Entity::drawClickBox() {
	float posX = m_position[0], posY = m_position[1];
	float width = 32.0f, height = 32.0f;

}