#include "GameMap.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "GameConstants.h"

#include "Cell2D.h"
#include "ObjectData.h"

#include "Base.h"
#include "Unit.h"
#include "ObjectPath.h"

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

	obj->UpdateGraphics();

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

Cell2D GameMap::GetNewUnitDestination(GameObject * gen) {

	const int r1 = gen->GetRow1() > 0 ? gen->GetRow1() - 1 : 0;
	const int c1 = gen->GetCol1() > 0 ? gen->GetCol1() - 1 : 0;
	const int r0 = gen->GetRow0() < static_cast<int>(m_rows - 1) ? gen->GetRow0() + 1 : m_rows - 1;
	const int c0 = gen->GetCol0() < static_cast<int>(m_cols - 1) ? gen->GetCol0() + 1 : m_cols - 1;

	const int indBaseTop = r1 * m_cols;
	const int indBaseBottom = r0 * m_cols;

	const int halfRows = m_rows / 2;
	const int halfCols = m_cols / 2;

	// NOTE for simplicity corner cells are overlapping and sometimes checked twice.
	// This can be optimized, but it's probably not worth it for now.


	// BOTTOM of the map
	if (r0 > halfRows)
	{
		// LEFT of the map
		if (c0 < halfCols)
		{
			// check right (top to bottom)
			for (int r = r1; r <= r0; ++r)
			{
				if (mCells[r * m_cols + c0].walkable)
					return Cell2D(r, c0);
			}

			// check top (right to left)
			for (int c = c0; c >= c1; --c)
			{
				if (mCells[indBaseTop + c].walkable)
					return Cell2D(r1, c);
			}

			// check bottom (right to left)
			for (int c = c0; c >= c1; --c)
			{
				if (mCells[indBaseBottom + c].walkable)
					return Cell2D(r0, c);
			}

			// check left (top to bottom)
			for (int r = r1; r <= r0; ++r)
			{
				if (mCells[r * m_cols + c1].walkable)
					return Cell2D(r, c1);
			}
		}
		// RIGHT of the map
		else
		{
			// check left (top to bottom)
			for (int r = r1; r <= r0; ++r)
			{
				if (mCells[r * m_cols + c1].walkable)
					return Cell2D(r, c1);
			}

			// check top (left to right)
			for (int c = c1; c <= c0; ++c)
			{
				if (mCells[indBaseTop + c].walkable)
					return Cell2D(r1, c);
			}

			// check right (top to bottom)
			for (int r = r1; r <= r0; ++r)
			{
				if (mCells[r * m_cols + c0].walkable)
					return Cell2D(r, c0);
			}

			// check bottom (left to right)
			for (int c = c1; c <= c0; ++c)
			{
				if (mCells[indBaseBottom + c].walkable)
					return Cell2D(r0, c);
			}
		}
	}
	// TOP of the map
	else
	{
		// LEFT of the map
		if (c0 < halfCols)
		{
			// check right (bottom to top)
			for (int r = r0; r >= r1; --r)
			{
				if (mCells[r * m_cols + c0].walkable)
					return Cell2D(r, c0);
			}

			// check bottom (right to left)
			for (int c = c0; c >= c1; --c)
			{
				if (mCells[indBaseBottom + c].walkable)
					return Cell2D(r0, c);
			}

			// check top (right to left)
			for (int c = c0; c >= c1; --c)
			{
				if (mCells[indBaseTop + c].walkable)
					return Cell2D(r1, c);
			}

			// check left (bottom to top)
			for (int r = r0; r >= r1; --r)
			{
				if (mCells[r * m_cols + c1].walkable)
					return Cell2D(r, c1);
			}
		}
		// RIGHT of the map
		else
		{
			// check left (bottom to top)
			for (int r = r0; r >= r1; --r)
			{
				if (mCells[r * m_cols + c1].walkable)
					return Cell2D(r, c1);
			}

			// check bottom (left to right)
			for (int c = c1; c <= c0; ++c)
			{
				if (mCells[indBaseBottom + c].walkable)
					return Cell2D(r0, c);
			}

			// check top (left to right)
			for (int c = c1; c <= c0; ++c)
			{
				if (mCells[indBaseTop + c].walkable)
					return Cell2D(r1, c);
			}

			// check right (bottom to top)
			for (int r = r0; r >= r1; --r)
			{
				if (mCells[r * m_cols + c0].walkable)
					return Cell2D(r, c0);
			}
		}
	}

	// failed to find a spot
	return Cell2D(-1, -1);
}

void GameMap::StartCreateUnit(const ObjectData & data, GameObject * gen, const Cell2D & dest) {
	const int ind = dest.row * m_cols + dest.col;
	GameMapCell & gcell = mCells[ind];

	// mark cell as changing
	gcell.changing = true;

	// mark generator as busy
	gen->SetBusy(true);
}

void GameMap::CreateUnit(const ObjectData & data, GameObject * gen, const Cell2D & dest) {
	const unsigned int r = static_cast<unsigned int>(dest.row);
	const unsigned int c = static_cast<unsigned int>(dest.col);

	const int ind = r * m_cols + c;
	GameMapCell & gcell = mCells[ind];

	Unit * unit = new Unit(data, 1, 1);
	unit->UpdateGraphics();
	//unit->SetOwner(player); # calls UpdateGraphics || calls RepositionObject
	unit->SetCell(&mCells[ind]);

	// links to other objects
	unit->SetGameMap(this);
	
	//unit->SetScreen(mScreenGame);

	// update cell
	gcell.objTop = unit;
	gcell.walkable = false;
	gcell.changing = false;

	mIsoMap->GetLayer(3)->AddObject(unit->GetIsoObject(), r, c);

	
	// store unit in map list and in registry
	mObjects.push_back(unit);
	mObjectsSet.insert(unit);

	// update generator, if any
	if (gen)
		gen->SetBusy(false);
}

bool GameMap::IsCellWalkable(unsigned int r, unsigned int c) const{
	const unsigned int ind = r * m_cols + c;

	return mCells[ind].walkable;
}

bool GameMap::MoveUnit(ObjectPath * path){
	GameObject * obj = path->GetObject();

	const int ind = obj->GetRow0() * m_cols + obj->GetCol0();

	// object is not in its cell !?
	if (mCells[ind].objTop != obj)
		return false;

	// start path
	path->Start();
	const bool started = path->GetState() == ObjectPath::RUNNING;

	if (started)
		mPaths.emplace_back(path);

	return started;
}

Cell2D GameMap::GetCloseMoveTarget(const Cell2D & start, const Cell2D & end) const{
	// get all walkable cells around end
	const int rowTL = end.row - 1 > 0 ? end.row - 1 : 0;
	const int colTL = end.col - 1 > 0 ? end.col - 1 : 0;
	const int rowBR = end.row + 1 < static_cast<int>(m_rows - 1) ? end.row + 1 : m_rows - 1;
	const int colBR = end.col + 1 < static_cast<int>(m_cols - 1) ? end.col + 1 : m_cols - 1;

	std::vector<Cell2D> walkalbes;
	const int maxWalkables = 8;
	walkalbes.reserve(maxWalkables);

	for (int r = rowTL; r <= rowBR; ++r){
		const int indBase = r * m_cols;

		for (int c = colTL; c <= colBR; ++c){
			const int ind = indBase + c;

			if (mCells[ind].walkable)
				walkalbes.emplace_back(r, c);
		}
	}

	return GetClosestCell(start, walkalbes);
}

Cell2D GameMap::GetClosestCell(const Cell2D & start, const std::vector<Cell2D> targets) const{
	// failed to find any walkable
	if (targets.empty())
		return Cell2D(-1, -1);

	// get closest cell
	int minInd = 0;
	int minDist = std::abs(start.row - targets[minInd].row) +
		std::abs(start.col - targets[minInd].col);

	for (unsigned int i = 1; i < targets.size(); ++i){
		const int dist = std::abs(start.row - targets[i].row) +
			std::abs(start.col - targets[i].col);

		if (dist < minDist){
			minDist = dist;
			minInd = i;
		}
	}

	return targets[minInd];
}

bool GameMap::AreObjectsAdjacent(const GameObject * obj1, const GameObject * obj2) const{
	// expand obj1 area by 1
	const int expRowTL = obj1->GetRow1() - 1;
	const int expColTL = obj1->GetCol1() - 1;
	const int expRowBR = obj1->GetRow0() + 1;
	const int expColBR = obj1->GetCol0() + 1;

	// check if expanded area and obj2 intersect
	return expRowTL <= obj2->GetRow0() &&
		obj2->GetRow1() <= expRowBR &&
		expColTL <= obj2->GetCol0() &&
		obj2->GetCol1() <= expColBR;
}

Cell2D GameMap::GetAdjacentMoveTarget(const Cell2D & start, const GameObject * target) const{
	const Cell2D tl(target->GetRow1(), target->GetCol1());
	const Cell2D br(target->GetRow0(), target->GetCol0());

	return GetAdjacentMoveTarget(start, tl, br);
}

Cell2D GameMap::GetAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL, const Cell2D & targetBR) const{
	// get all walkable cells around target
	const int tRows = targetBR.row - targetTL.row + 1;
	const int tCols = targetBR.col - targetTL.col + 1;

	const int rowTL = targetTL.row - 1 > 0 ? targetTL.row - 1 : 0;
	const int colTL = targetTL.col - 1 > 0 ? targetTL.col - 1 : 0;
	const int rowBR = targetBR.row + 1 < static_cast<int>(m_rows - 1) ? targetBR.row + 1 : m_rows - 1;
	const int colBR = targetBR.col + 1 < static_cast<int>(m_cols - 1) ? targetBR.col + 1 : m_cols - 1;

	std::vector<Cell2D> walkalbes;
	const int maxWalkables = (tCols + 2) * 2 + tRows;
	walkalbes.reserve(maxWalkables);

	for (int r = rowTL; r <= rowBR; ++r){
		const int indBase = r * m_cols;

		for (int c = colTL; c <= colBR; ++c){
			const int ind = indBase + c;

			if (mCells[ind].walkable)
				walkalbes.emplace_back(r, c);
		}
	}

	return GetClosestCell(start, walkalbes);
}

void GameMap::update(float delta){
	// -- game objects --
	auto itObj = mObjects.begin();

	while (itObj != mObjects.end()){
		GameObject * obj = *itObj;

		obj->Update(delta);

		if (obj->IsDestroyed()){
			GameObject * obj = *itObj;

			// erase object from vector and set
			itObj = mObjects.erase(itObj);
			mObjectsSet.erase(obj);

			//DestroyObject(obj);
		}
		else
			++itObj;
	}

	//for (CollectableGenerator * dg : mCollGen)
		//dg->Update(delta);

	// paths
	UpdateObjectPaths(delta);

	// conquer paths
	//UpdateConquerPaths(delta);

	// wall building paths
	//UpdateWallBuildPaths(delta);
}

void GameMap::UpdateObjectPaths(float delta){
	auto itPath = mPaths.begin();

	while (itPath != mPaths.end()){
		ObjectPath * path = *itPath;

		path->Update(delta);

		const ObjectPath::PathState state = path->GetState();

		if (state == ObjectPath::PathState::COMPLETED || state == ObjectPath::PathState::ABORTED){
			delete path;
			itPath = mPaths.erase(itPath);
		}else if (state == ObjectPath::PathState::FAILED){
			// TODO try to recover from failed path
			delete path;
			itPath = mPaths.erase(itPath);
		}else
			++itPath;
	}
}