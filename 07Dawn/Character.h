#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <complex>
#include <tuple>
#include <iostream>

#include "TextureManager.h"

class TileSet;

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

	void setNumMoveTexturesPerDirection(ActivityType::ActivityType activity, int numTextures);
	void setMoveTexture(ActivityType::ActivityType activity, Direction direction, int index, std::string filename, int textureOffsetX = 0, int textureOffsetY = 0);
	TileSet* getTileSet(ActivityType::ActivityType activity, Direction direction);
	void setTileSet(std::map<std::pair<int, int>, TileSet>& moveTileSets);
	ActivityType::ActivityType getCurActivity() const;
	int getXPos() const;
	int getYPos() const;
	void setName(std::string newName);
	unsigned short getNumActivities();

	bool alive;
	//bool hasDrawnDyingOnce;
	int current_texture, direction_texture;

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

	int* numMoveTexturesPerDirection;


	std::map<std::pair<int, int>, TileSet> m_moveTileSets;

	unsigned int m_textureAtlas;
	int x_pos, y_pos;
	std::string name;
};