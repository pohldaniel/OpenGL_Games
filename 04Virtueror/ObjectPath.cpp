#include "ObjectPath.h"

#include "Game.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "GameObject.h"
#include "Unit.h"


#include <cmath>


void ObjectPath::InitNextMoveStep(){
	const IsoObject * isoObj = mObj->GetIsoObject();
	const IsoLayer * layerObj = isoObj->GetLayer();

	mObjX = isoObj->GetX();
	mObjY = isoObj->GetY();

	const unsigned int nextInd = mCells[mNextCell];
	const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
	const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();
	Vector2f target = layerObj->GetObjectPosition(isoObj, nextRow, nextCol);
	mTargetX = target[0];
	mTargetY = target[1];

	mVelX = (mTargetX - mObjX) * mObj->GetSpeed();
	mVelY = (mTargetY - mObjY) * mObj->GetSpeed();

	++mNextCell;
}

void ObjectPath::Start(){
	// do nothing if already started
	if (mState != READY)
		return;

	mNextCell = 1;

	const unsigned int nextInd = mCells[mNextCell];
	const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
	const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();

	// check if next destination is walkable
	const GameMapCell & nextCell = mGameMap->GetCell(nextRow, nextCol);

	bool canMove = nextCell.walkable;

	// TODO remove check if mObj is changed into mUnit like for other paths
	if (mObj->GetObjectType() == OBJ_UNIT)
		canMove = canMove && static_cast<Unit *>(mObj)->HasEnergyForAction(MOVE);

	if (canMove){
		mState = RUNNING;

		InitNextMoveStep();
	}else{
		mState = FAILED;

		// clear action data once the action is completed
		mGame->SetObjectActionCompleted(mObj);
	}
}

void ObjectPath::Update(float delta){
	int todo = 2;
	// -- X --
	mObjX += mVelX * delta;

	if (mVelX < 0.f){

		if (mObjX < mTargetX){
			--todo;
			mObjX = mTargetX;
		}
	}else if (mVelX > 0.f){

		if (mObjX > mTargetX){
				--todo;
				mObjX = mTargetX;
		}
	}else
		--todo;

	// -- Y --
	mObjY += mVelY * delta;

	if (mVelY < 0.f){

		if (mObjY < mTargetY){
			--todo;
			mObjY = mTargetY;
		}
	}else if (mVelY > 0.f){

		if (mObjY > mTargetY){
			--todo;
			mObjY = mTargetY;
		}
	}else
		--todo;

	// position object
	IsoObject * isoObj = mObj->GetIsoObject();
	isoObj->SetPosition(std::roundf(mObjX), std::roundf(mObjY));


	// handle reached target
	if (0 == todo){
		//Player * player = mObj->GetOwner();

		//mGameMap->DelPlayerObjVisibility(mObj, player);

		const unsigned int targetInd = mCells[mNextCell - 1];
		const unsigned int targetRow = targetInd / mIsoMap->GetNumCols();
		const unsigned int targetCol = targetInd % mIsoMap->GetNumCols();

		const GameMapCell & targetCell = mGameMap->GetCell(targetRow, targetCol);

		// collect collectable object, if any
		/*if (targetCell.walkable && targetCell.objTop != nullptr){
			player->HandleCollectable(targetCell.objTop);
			mGameMap->RemoveAndDestroyObject(targetCell.objTop);
		}*/

		// handle moving object
		mGameMap->MoveObjToCell(mObj, targetRow, targetCol);
		//mGameMap->AddPlayerObjVisibility(mObj, player);
		//mGameMap->ApplyVisibility(player);

		// TODO remove check if mObj is changed into mUnit like for other paths
		if (mObj->GetObjectType() == OBJ_UNIT)
			static_cast<Unit *>(mObj)->ConsumeEnergy(MOVE);

		// handle next step or termination
		if (ABORTING == mState){
			mState = ABORTED;

			// clear action data once the action is completed
			mGame->SetObjectActionCompleted(mObj);
		}else if (mNextCell < mCells.size()){

			const unsigned int nextInd = mCells[mNextCell];
			const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
			const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();

			// check if next destination is walkable
			const GameMapCell & nextCell = mGameMap->GetCell(nextRow, nextCol);

			bool canMove = nextCell.walkable;

			// TODO remove check if mObj is changed into mUnit like for other paths
			if (mObj->GetObjectType() == OBJ_UNIT)
				canMove = canMove && static_cast<Unit *>(mObj)->HasEnergyForAction(MOVE);

			if (canMove)
				InitNextMoveStep();
			else{
				mState = FAILED;

				// clear action data once the action is completed
				mGame->SetObjectActionCompleted(mObj);
			}
		}else{
			mState = COMPLETED;

			// clear action data once the action is completed
			mGame->SetObjectActionCompleted(mObj);

			mOnCompleted();
		}
	}
}

void ObjectPath::UpdatePathCost(){
	// TODO proper cost computation
	mCost = (mCells.size() - 1) * 0.5f;
}

bool GameMap::MoveObjToCell(GameObject * obj, int row, int col){
	// TODO support objects covering more than 1 cell
	const int ind0 = obj->GetRow0() * m_cols + obj->GetCol0();

	// object is not in its cell !?
	if (mCells[ind0].objTop != obj)
		return false;

	// update minimap
	//MiniMap * mm = mScreenGame->GetMiniMap();
	//mm->MoveElement(obj->GetRow0(), obj->GetCol0(), row, col);

	// move object in iso map
	IsoLayer * layer = obj->GetIsoObject()->GetLayer();
	layer->MoveObject(obj->GetRow0(), obj->GetCol0(), row, col, false);

	// remove object from current cell
	mCells[ind0].objTop = nullptr;
	mCells[ind0].walkable = true;

	// add object to new cell
	const int ind1 = row * m_cols + col;

	obj->SetCell(&mCells[ind1]);

	mCells[ind1].objTop = obj;
	mCells[ind1].walkable = false;

	return true;
}
