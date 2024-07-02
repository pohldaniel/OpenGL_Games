#pragma once

#include <Entities/SpriteEntity.h>

class Player;

class Character : public SpriteEntity {

public:

	Character(Cell& cell, const Rect& collisionRect);
	virtual ~Character();

	void update(float dt) override;

	void setCharacterId(const std::string& characterId);
	void setRadius(float radius);
	const std::string& getCharacterId();

	void startMove();
	bool raycast(const Player& player);
	void setRayCast(bool rayCast);

private:

	std::string m_characterId;
	float m_radius;
	bool m_move;
	bool m_rayCast;
	const Rect& collisionRect;
};