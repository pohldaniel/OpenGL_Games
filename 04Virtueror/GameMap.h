#pragma once
#include <vector>
#include <unordered_set>

#include "GameObject.h"
#include "GameMapCell.h"

struct Cell2D;
struct ObjectData;

class IsoMap;
class Unit;

class GameMap {
public:
	GameMap(IsoMap * isoMap);
	~GameMap();

	void CreateObjectFromFile(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols);
	void CreateObject(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols);
	void UpdateCellType(unsigned int ind, const GameMapCell& cell, bool mapBuffer);

	int DefineCellType(unsigned int ind, const GameMapCell& cell);
	void SetSize(unsigned int rows, unsigned int cols);
	const GameMapCell & GetCell(unsigned int r, unsigned int c) const;

	// unit create
	Cell2D GetNewUnitDestination(GameObject * gen);
	//void StartCreateUnit(const ObjectData & data, GameObject * gen, const Cell2D & dest, Player * player);
	void StartCreateUnit(const ObjectData & data, GameObject * gen, const Cell2D & dest);
	//void CreateUnit(const ObjectData & data, GameObject * gen, const Cell2D & dest, Player * player);
	void CreateUnit(const ObjectData & data, GameObject * gen, const Cell2D & dest);

	unsigned int m_rows = 0;
	unsigned int m_cols = 0;

	std::vector<GameMapCell> mCells;
	std::vector<GameObject *> mObjects;
	std::unordered_set<GameObject *> mObjectsSet;

	IsoMap * mIsoMap = nullptr;

	bool HasObject(unsigned int r, unsigned int c) const;
	bool HasObject(GameObject * obj) const;

	
};

inline bool GameMap::HasObject(unsigned int r, unsigned int c) const{
	const unsigned int ind = r * m_cols + c;

	//return ind < mCells.size() && mCells[ind].objTop != nullptr;

	return true;
}

inline bool GameMap::HasObject(GameObject * obj) const{
	return mObjectsSet.find(obj) != mObjectsSet.end();
}

inline const GameMapCell & GameMap::GetCell(unsigned int r, unsigned int c) const{
	return mCells[r * m_cols + c];
}