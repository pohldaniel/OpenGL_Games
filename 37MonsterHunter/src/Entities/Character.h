#pragma once
#include <functional>
#include <Entities/SpriteEntity.h>

class Player;

class Character : public SpriteEntity {

public:

	Character(Cell& cell, Rect& rect);
	virtual ~Character();

	void update(float dt) override;

	void setCharacterId(const std::string& characterId);
	void setRadius(float radius);
	void setRayCast(bool rayCast);
	void setDefeated(bool defeated);
	void setCollisionRectIndex(int collisionRectIndex);
	void setOnMoveEnd(std::function<void()> fun);

	const std::string& getCharacterId();
	void startMove(const Vector2f& playerPos);
	bool raycast(const Player& player);
	bool isDefeated();

private:

	bool hasLineOfSight(const Cell& cell, const std::vector<Rect>& collisionRects, float radius) const;

	std::string m_characterId;
	float m_radius;
	bool m_move;
	bool m_rayCast;
	bool m_defeated;

	int m_collisionRectIndex;
	Rect& collisionRect;
	Vector2f m_playerPos;
	std::function<void()> OnMoveEnd;
};