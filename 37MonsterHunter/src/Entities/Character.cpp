#include <iostream>
#include <string>
#include <Entities/Player.h>
#include "Character.h"

Character::Character(Cell& cell) : SpriteEntity(cell), m_characterId(""), m_radius(0.0f), m_move(false), m_rayCast(true), m_collisionRectIndex(-1){

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

//https://www.jeffreythompson.org/collision-detection/line-rect.php
bool lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
	// calculate the direction of the lines
	float uA = ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	float uB = ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		return true;
	}
	return false;
}

bool lineRect(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh) {
	// check if the line has hit any of the rectangle's sides
	// uses the Line/Line function below
	bool left = lineLine(x1, y1, x2, y2, rx, ry, rx, ry + rh);
	bool right = lineLine(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh);
	bool top = lineLine(x1, y1, x2, y2, rx, ry, rx + rw, ry);
	bool bottom = lineLine(x1, y1, x2, y2, rx, ry + rh, rx + rw, ry + rh);

	// if ANY of the above are true, the line
	// has hit the rectangle
	return (left || right || top || bottom);
}

bool Character::hasLineOfSight(const Cell& cell, const std::vector<Rect>& collisionRects, float radius) const {
	if (Vector2f::Length(Vector2f(cell.centerX, cell.centerY), Vector2f(Character::cell.centerX, Character::cell.centerY)) < radius) {
		for (size_t i = 0; i < collisionRects.size(); ++i) {
			if (i == m_collisionRectIndex)
				continue;
			const Rect& rect = collisionRects[i];
			if (lineRect(cell.centerX, cell.centerY, Character::cell.centerX, Character::cell.centerY, rect.posX, rect.posY, rect.width, rect.height)) {
				return false;
			}
		}
	}
	return true;
}

bool Character::raycast(const Player& player) {
	return m_rayCast && CheckConnection(getCell(), player.getCell(), m_viewDirection, m_radius, 30.0f) && hasLineOfSight(player.getCell(), player.getCollisionRects(), m_radius);
}

void Character::setRayCast(bool rayCast) {
	m_rayCast = rayCast;
}

void Character::setCollisionRectIndex(int collisionRectIndex) {
	m_collisionRectIndex = collisionRectIndex;
}