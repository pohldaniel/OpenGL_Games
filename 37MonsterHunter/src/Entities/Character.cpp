#include <iostream>
#include <string>
#include "Character.h"

Character::Character(Cell& cell) : SpriteEntity(cell), m_characterId(""), m_radius(0.0f), m_move(false) {

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

bool Character::checkConnection(const Cell& origin, const Cell& target, ViewDirection viewDirection, float radius, float tolerance) {
	Vector2f relation = Vector2f(target.centerX - origin.centerX, target.centerY - origin.centerY);
	if (relation.length() < radius) {
		if ((viewDirection == ViewDirection::LEFT && relation[0] < 0.0f && fabs(relation[1]) < tolerance) ||
			(viewDirection == ViewDirection::RIGHT && relation[0] > 0.0f && fabs(relation[1]) < tolerance) ||
			(viewDirection == ViewDirection::UP && relation[1] < 0.0f && fabs(relation[0]) < tolerance) ||
			(viewDirection == ViewDirection::DOWN && relation[1] > 0.0f && fabs(relation[0]) < tolerance)
			){
			return true;
		}
	}
	return false;
}

void Character::changeFacingDirection(const SpriteEntity& target) {
	Vector2f relation = Vector2f(cell.centerX - target.getCell().centerX, cell.centerY - target.getCell().centerY);
	if (fabs(relation[1]) < 30)
		relation[0] > 0 ? setViewDirection(ViewDirection::LEFT) : setViewDirection(ViewDirection::RIGHT);
	else
		relation[1] > 0 ? setViewDirection(ViewDirection::UP) : setViewDirection(ViewDirection::DOWN);
}

void Character::startMove() {
	m_move = true;
}

bool Character::raycast(const Cell& target) {
	return checkConnection(getCell(), target, m_viewDirection, m_radius, 30.0f);
}