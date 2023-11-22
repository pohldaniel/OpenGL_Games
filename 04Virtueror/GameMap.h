#pragma once
#include <vector>
#include <unordered_set>

#include "IPathMap.h"

#include "GameObject.h"
#include "GameMapCell.h"

struct Cell2D;
struct ObjectData;

class IsoMap;
class Unit;
class ObjectPath;

class GameMap : public IPathMap {
public:
	GameMap(IsoMap * isoMap);
	~GameMap();

	void update(float delta);

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
	std::vector<ObjectPath *> mPaths;

	IsoMap * mIsoMap = nullptr;

	bool HasObject(unsigned int r, unsigned int c) const;
	bool HasObject(GameObject * obj) const;

	bool IsCellWalkable(unsigned int r, unsigned int c) const override;

	unsigned int GetNumRows() const;
	unsigned int GetNumCols() const;

	bool MoveObjToCell(GameObject * obj, int row, int col);
	bool MoveUnit(ObjectPath * path);
	Cell2D GetCloseMoveTarget(const Cell2D & start, const Cell2D & end) const;
	Cell2D GetClosestCell(const Cell2D & start, const std::vector<Cell2D> targets) const;
	bool AreObjectsAdjacent(const GameObject * obj1, const GameObject * obj2) const;
	Cell2D GetAdjacentMoveTarget(const Cell2D & start, const GameObject * target) const;
	Cell2D GetAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL, const Cell2D & targetBR) const;

	void GameMap::UpdateObjectPaths(float delta);
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

inline unsigned int GameMap::GetNumRows() const { return m_rows; }

inline unsigned int GameMap::GetNumCols() const { return m_cols; }


