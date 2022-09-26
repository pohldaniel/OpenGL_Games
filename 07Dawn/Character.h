#pragma once

#include <cstdint>
#include <utility>
#include <unordered_map>
#include <iostream>

#include "TextureManager.h"
#include "Random.h"

class TileSet;

struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2> &p) const {
		return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
	}
};

enum Direction{
	STOP = 0,
	N = 1,
	NE = 2,
	E = 3,
	SE = 4,
	S = 5,
	SW = 6,
	W = 7,
	NW = 8
};

namespace ActivityType{

	enum ActivityType{
		Walking,
		Casting,
		Attacking,
		Shooting,
		Dying,
		Count
	};
}

namespace Attitude{

	enum Attitude{
		FRIENDLY,
		NEUTRAL,
		HOSTILE
	};
}

class Character{

public:

	Character();
	~Character() = default;

	virtual void draw() {}
	void update(float deltaTime);

	void setMoveTexture(ActivityType::ActivityType activity, Direction direction, int index, std::string filename, int textureOffsetX = 0, int textureOffsetY = 0);
	TileSet* getTileSet(ActivityType::ActivityType activity, Direction direction);
	void setTileSet(std::unordered_map<std::pair<int, int>, TileSet, pair_hash>& moveTileSets);
	void setNumActivities(unsigned short numActivities);

	ActivityType::ActivityType getCurActivity() const;
	int getXPos() const;
	int getYPos() const;
	void setName(std::string newName);
	unsigned short getNumActivities();
	virtual Direction GetDirection() = 0;

	Direction GetDirectionTexture();
	uint16_t getWanderRadius() const;
	float getMovementSpeed() const;

	bool isStunned() const;
	bool isMesmerized() const;
	bool isFeared() const;
	bool isCharmed() const;
	bool getIsPreparing() const;
	bool mayDoAnythingAffectingSpellActionWithoutAborting() const;
	bool mayDoAnythingAffectingSpellActionWithAborting() const;
	bool isPlayer() const;
	bool isChanneling() const;
	bool isSneaking() const;

	void Move();
	bool continuePreparing();
	void MoveUp(uint8_t n);
	void MoveDown(uint8_t n);
	void MoveLeft(uint8_t n);
	void MoveRight(uint8_t n);

	bool alive;
	//bool hasDrawnDyingOnce;
	int current_texture, direction_texture;
	bool hasChoosenFearDirection;
	bool isPreparing;

	// timers
	float wander_thisframe, wander_lastframe;
	float respawn_thisframe, respawn_lastframe;
	float dyingStartFrame, reduceDyingTranspFrame;

	int wander_every_seconds, wander_points_left;
	bool do_respawn;
	int x_spawn_pos, y_spawn_pos;
	int NPC_id;
	int seconds_to_respawn;
	bool wandering, moving, in_combat;
	uint32_t remainingMovePoints;


	unsigned short m_numActivities;
	std::unordered_map<std::pair<int, int>, TileSet, pair_hash> m_moveTileSets;

	int x_pos, y_pos;
	std::string name;
	uint16_t wander_radius;
	Direction activeDirection;
	Direction WanderDirection, MovingDirection, fearDirection, dyingDirection;


	TextureRect* rect;
	int index = 0;
	float dumping = 0.0f;
};