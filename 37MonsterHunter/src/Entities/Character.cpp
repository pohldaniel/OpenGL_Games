#include <iostream>
#include <string>
#include <Entities/Player.h>
#include "Character.h"

Character::Character(Cell& cell, const Rect& collisionRect) : SpriteEntity(cell), collisionRect(collisionRect), m_characterId(""), m_radius(0.0f), m_move(false), m_rayCast(true){

}

Character::~Character() {

}

void Character::update(float dt) {
	if (!m_move)
		return;

	cell.posX += m_direction[0] * dt * m_movingSpeed;
	cell.posY -= m_direction[1] * dt * m_movingSpeed;
	cell.centerX = cell.posX + 64.0f;
	cell.centerY = cell.posY - 64.0f;

	updateAnimation(dt);
}

void Character::setCharacterId(const std::string& characterId) {
	m_characterId = characterId;
}

void Character::setRadius(float radius) {
	m_radius = radius;
}

const std::string& Character::getCharacterId() {
	return m_characterId;
}

void Character::startMove() {
	m_move = true;
}

bool Character::raycast(const Player& player) {
	return m_rayCast && CheckConnection(getCell(), player.getCell(), m_viewDirection, m_radius, 30.0f) && player.hasLineOfSight(getCell(), collisionRect, m_radius);
}

void Character::setRayCast(bool rayCast) {
	m_rayCast = rayCast;
}