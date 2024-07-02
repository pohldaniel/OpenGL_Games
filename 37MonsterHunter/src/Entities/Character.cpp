#include <iostream>
#include <string>
#include "Character.h"

Character::Character(Cell& cell) : SpriteEntity(cell), m_characterId(""), m_radius(0.0f) {

}

Character::~Character() {

}

void Character::update(float dt) {
	bool move = false;

	if (!move)
		return;
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

void Character::checkConnection(const SpriteEntity& target, float tolerance) {
	Vector2f relation = Vector2f(target.getCell().centerX - cell.centerX, target.getCell().centerY - cell.centerY);
	if (relation.length() < m_radius) {
		std::cout << "ID: " << m_characterId << std::endl;
	}
}