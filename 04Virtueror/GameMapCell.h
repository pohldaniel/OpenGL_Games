#pragma once
#include <unordered_map>

enum CellTypes : int{
	EMPTY = 0,
	SCENE_ROCKS,
	SCENE_ROCKS_SURR,
	SCENE_RESOURCE,
	BLOBS_SOURCE,
	DIAMONDS_SOURCE,
	BLOBS_SURR,
	DIAMONDS_SURR,

	// FACTION 1
	F1,
	F1_CONNECTED,
	F1_INFLUENCED,

	// FACTION 2
	F2,
	F2_CONNECTED,
	F2_INFLUENCED,

	// FACTION 3
	F3,
	F3_CONNECTED,
	F3_INFLUENCED,

	// SPECIAL
	FOG_OF_WAR,

	NUM_CELL_TYPES
};

class GameObject;

struct GameMapCell{
	GameMapCell();

	//bool HasUnit() const;
	//bool HasResourceGenerator() const;

	std::unordered_map<int, bool> influencers;
	
	GameObject * objBottom = nullptr;
	GameObject * objTop = nullptr;

	CellTypes currType = EMPTY;
	CellTypes basicType = EMPTY;

	int row = 0;
	int col = 0;
	bool walkable = true;
	bool changing = false;
	bool linked = false;
};