#pragma once

#include <Entities/SpriteEntity.h>

class Player;

class Character : public SpriteEntity {

public:

	Character(Cell& cell);
	virtual ~Character();

	void update(float dt) override;

	void setCharacterId(const std::string& characterId);
	void setRadius(float radius);
	const std::string& getCharacterId();

	void startMove();
	bool raycast(const Player& player);
	void setRayCast(bool rayCast);
	void setCollisionRectIndex(int collisionRectIndex);

private:

	bool hasLineOfSight(const Cell& cell, const std::vector<Rect>& collisionRects, float radius) const;

	std::string m_characterId;
	float m_radius;
	bool m_move;
	bool m_rayCast;
	int m_collisionRectIndex;
};