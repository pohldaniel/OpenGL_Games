#pragma once
#include <vector>

class IsoMap;
struct GameMapCell;

class GameMap {
public:
	GameMap(IsoMap * isoMap);
	~GameMap();

	void CreateObjectFromFile(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols);
	void CreateObject(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols);
	void UpdateCellType(unsigned int ind, const GameMapCell& cell, bool mapBuffer);

	int DefineCellType(unsigned int ind, const GameMapCell& cell);
	void SetSize(unsigned int rows, unsigned int cols);

	unsigned int m_rows = 0;
	unsigned int m_cols = 0;
	std::vector<GameMapCell> mCells;
	IsoMap * mIsoMap = nullptr;

	
};