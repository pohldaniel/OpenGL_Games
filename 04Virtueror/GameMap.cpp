#include "GameMap.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "GameConstants.h"

#include "Base.h"

#include <iostream>

GameMap::GameMap(IsoMap * isoMap) {
	mIsoMap = isoMap;
	//SetSize(isoMap->GetNumRows(), isoMap->GetNumCols());
}

GameMap::~GameMap() {

}

void GameMap::SetSize(unsigned int rows, unsigned int cols){
	const unsigned int size = rows * cols;

	if (size == mCells.size())
		return;

	m_rows = rows;
	m_cols = cols;

	mCells.resize(size);

	// set cell coordinates
	int index = 0;

	for (unsigned int r = 0; r < rows; ++r){
		for (unsigned int c = 0; c < cols; ++c){
			GameMapCell & cell = mCells[index++];
			cell.row = r;
			cell.col = c;
		}
	}

	// init player(s) visibility map
	// NOTE for now only for human player
	//mGame->GetPlayerByIndex(0)->InitVisibility(mRows, mCols);
}

void GameMap::CreateObjectFromFile(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols) {
	
	if (objId >= 1000 && objId < 2000) {
		CreateObject(layerId, 03, r0, c0, rows, cols);
	}
}

void GameMap::CreateObject(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols) {
	// object origin is out of map
	if (r0 >= m_rows || c0 >= m_cols)
		return;

	// full size is out of map
	const unsigned int r1 = 1 + r0 - rows;
	const unsigned int c1 = 1 + c0 - cols;

	if (r1 >= m_rows || c1 >= m_cols)
		return;

	const unsigned int ind0 = r0 * m_cols + c0;

	GameMapCell & gcell = mCells[ind0];

	// cell is already full
	//if (gcell.objTop)
		//return nullptr;

	// create game object
	//IsoObject* isoObj = new IsoObject(rows, cols);
	GameObject* obj = nullptr;

	obj = new Base(rows, cols);

	// set object properties
	obj->SetCell(&mCells[ind0]);
	obj->SetSize(rows, cols);

	// links to other objects
	obj->SetGameMap(this);

	mObjects.push_back(obj);
	mObjectsSet.insert(obj);

	// generic cells update
	for (unsigned int r = r1; r <= r0; ++r) {
		const unsigned int indBase = r * m_cols;

		for (unsigned int c = c1; c <= c0; ++c){

			const unsigned int ind = indBase + c;

			GameMapCell& cell = mCells[ind];

			cell.linked = true;
			cell.walkable = false;
			cell.objTop = obj;

			UpdateCellType(ind, cell, false);
		}
	}

	mIsoMap->mapBuffer();

	mIsoMap->GetLayer(layerId)->AddObject(obj->GetIsoObject(), r0, c0);
	obj->GetIsoObject()->SetPosition();
}

void GameMap::UpdateCellType(unsigned int ind, const GameMapCell & cell, bool mapBuffer){
	const int cellType = DefineCellType(ind, cell);
	mIsoMap->SetCellType(ind, cellType);
}

int GameMap::DefineCellType(unsigned int ind, const GameMapCell & cell){
	// if cell is not visible it's always Fog Of War
	//if (!mGame->GetLocalPlayer()->IsCellVisible(ind))
	//return FOG_OF_WAR;

	// scene cell
	if (SCENE_ROCKS == cell.currType || DIAMONDS_SOURCE == cell.currType || BLOBS_SOURCE == cell.currType)
		return cell.currType;

	const PlayerFaction ownerFaction = FACTION_1;

	int type = EMPTY;

	switch (ownerFaction)
	{
	case FACTION_1:
		if (cell.linked)
			type = F1_CONNECTED;
		else
			type = F1;
		break;

	case FACTION_2:
		if (cell.linked)
			type = F2_CONNECTED;
		else
			type = F2;
		break;

	case FACTION_3:
		if (cell.linked)
			type = F3_CONNECTED;
		else
			type = F3;
		break;

		// no owner
	default:{
		/*if (0 == cell.influencer)
			type = F1_INFLUENCED;
		else if (1 == cell.influencer)
			type = F2_INFLUENCED;
		else if (2 == cell.influencer)
			type = F3_INFLUENCED;
		// no influence
		else
			type = cell.basicType;*/
	}
	break;
	}

	return type;
}
