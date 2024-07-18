#pragma once
#include <functional>
#include <random>
#include <Entities/SpriteEntity.h>

#include "Timer.h"

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
	void setViewDirections(const std::vector<ViewDirection>& viewDirections);
	void stopLookAroundTimer();

	const std::string& getCharacterId();
	void startMove(const Vector2f& playerPos);
	bool raycast(const Player& player);
	bool isDefeated();

private:

	bool hasLineOfSight(const Cell& cell, const std::vector<Rect>& collisionRects, float radius) const;
	void randomViewDirection();

	std::string m_characterId;
	float m_radius;
	bool m_move;
	bool m_rayCast;
	bool m_defeated;
	std::vector<ViewDirection> m_viewDirections;

	Timer m_noticeTimer;
	Timer m_lookAroundTimer;
	int m_collisionRectIndex;
	Rect& collisionRect;
	Vector2f m_playerPos;
	std::function<void()> OnMoveEnd;

	static std::random_device RandomDevice;
	static std::mt19937 MersenTwist;
};