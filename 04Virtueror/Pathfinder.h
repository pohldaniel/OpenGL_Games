#pragma once

#include <cstdlib>

#include <unordered_map>
#include <vector>

class IPathMap;
class PathNode;

class Pathfinder{
public:
	enum PathOptions : unsigned int{
		INCLUDE_START = 0x1,
		ALLOW_DIAGONALS = 0x2,

		ALL_OPTIONS = INCLUDE_START | ALLOW_DIAGONALS
	};

public:
	~Pathfinder();

	void SetMap(const IPathMap * map, unsigned int mapRows, unsigned int mapCols);

	std::vector<unsigned int> MakePath(unsigned int r0, unsigned int c0,
				unsigned int r1, unsigned int c1,
				PathOptions options = INCLUDE_START);

private:
	unsigned int GetCellIndex(unsigned int r, unsigned int c) const;

	void AddToOpen(PathNode * PathNode, unsigned int index);

	int CostToAdj(int dr, int dc) const;
	int CostToGoal(int r0, int c0) const;

	void HandleNode(PathNode * prev, int dr, int dc);

	void ClearLists();

	void InitNodes();
	PathNode * GetNewNode(int row, int col, int g = 0, int h = 0, PathNode * parent = nullptr);

private:
	std::vector<PathNode *> mOpenList;
	std::unordered_map<unsigned int, PathNode *> mOpenMap;
	std::unordered_map<unsigned int, PathNode *> mClosedMap;

	PathNode * mNodes = nullptr;
	unsigned int mNextAvailableNode = 0;

	const IPathMap * mMap;
	unsigned int mMapRows = 0;
	unsigned int mMapCols = 0;
	unsigned int mMapSize = 0;

	int mR1;
	int mC1;

	int mCostHor = 10;
	int mCostDia = 14;
};

inline unsigned int Pathfinder::GetCellIndex(unsigned int r, unsigned int c) const{
	return (r * mMapCols) + c;
}

inline int Pathfinder::CostToAdj(int dr, int dc) const{
	if (dr != 0 && dc != 0)
		return mCostDia;
	else
		return mCostHor;
}

inline int Pathfinder::CostToGoal(int r0, int c0) const{
	return (abs(r0 - mR1) + abs(c0 - mC1)) * mCostHor;
}