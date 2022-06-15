#include <iostream>

#include "IsoMap.h"

#include "IsoLayer.h"
#include "IsoObject.h"

#include <algorithm>


IsoLayer::IsoLayer(const IsoMap * map) : mMap(map) {

	const int size = 40 * 40;

	mObjectsMap.reserve(size);
	mObjectsMap.assign(size, nullptr);

	mObjectsList.reserve(size);
	mRenderList.reserve(size);
}

// ==================== PUBLIC METHODS ====================

void IsoLayer::UpdateSize()
{
	const unsigned int size = 40 * 40;

	if (size == mObjectsMap.size())
		return;

	mObjectsMap.resize(size);

	// update objects list
	//mRenderList.clear();

	for (unsigned int ind = 0; ind < size; ++ind)
	{
		if (mObjectsMap[ind] != nullptr)
			mRenderList.emplace_back(mObjectsMap[ind]);
	}
}

/**
* @brief Checks if a cell contains an object.
* @param r Row index, starting from 0
* @param c Col index, starting from 0
* @return TRUE if cell contains an object, FALSE otherwise
*/
bool IsoLayer::ContainsObject(unsigned int r, unsigned int c) const
{
	const unsigned int rows = 40;
	const unsigned int cols = 40;

	// ERROR out of bounds
	if (!(r < rows && c < cols))
		return false;

	const unsigned int index = r * cols + c;

	return mObjectsMap[index] != nullptr;
}

bool IsoLayer::AddObject(IsoObject * obj, unsigned int r, unsigned int c)
{

	const unsigned int mapRows = 40;
	const unsigned int mapCols = 40;

	// object origin is out of map
	if (r >= mapRows || c >= mapCols)
		return false;

	// full size is out of map
	const unsigned int r1 = 1 + r - obj->GetRows();
	const unsigned int c1 = 1 + c - obj->GetCols();

	//if (r1 >= mapRows || c1 >= mapCols)
		//return false;

	// object already added
	//if (std::find(mObjectsList.begin(), mObjectsList.end(), obj) != mObjectsList.end())
		//return false;

	// link layer and object
	obj->SetLayer(this);

	// set object position in layer
	obj->SetRow(r);
	obj->SetCol(c);

	mObjectsList.emplace_back(obj);

	// position it in a cell
	//PositionObject(obj, r, c);

	// store object
	InsertObjectInMap(obj);

	UpdateRenderList();

	return true;
}

void IsoLayer::ClearObject(IsoObject * obj)
{
	if (nullptr == obj)
		return;

	obj->SetLayer(nullptr);

	ClearObjectFromMap(obj);
	RemoveObjectFromList(obj);
	RemoveObjectFromRenderList(obj);
}

/**
* @brief Destroys an object in a cell.
* @param r Row index, starting from 0
* @param c Col index, starting from 0
*/
void IsoLayer::ClearObject(unsigned int r, unsigned int c)
{
	const unsigned int rows = 40;
	const unsigned int cols = 40;

	// ERROR out of bounds
	if (!(r < rows && c < cols))
		return;

	const unsigned int index = r * cols + c;

	// empty cell -> nothing to do
	if (!mObjectsMap[index])
		return;

	ClearObject(index);
}

/**
* @brief Moves an object from one cell to another.
* @param r0 Row index of start cell, starting from 0
* @param c0 Col index of start cell, starting from 0
* @param r1 Row index of dest cell, starting from 0
* @param c1 Col index of start cell, starting from 0
* @param alignment Visual alignment of the object in the cell
* @return TRUE on success, FALSE otherwise
*/
bool IsoLayer::MoveObject(unsigned int r0, unsigned int c0,
	unsigned int r1, unsigned int c1,
	bool updatePosition)
{
	const unsigned int rows = 40;
	const unsigned int cols = 40;

	// ERROR out of bounds
	if (!(r0 < rows && c0 < cols && r1 < rows && c1 < cols))
		return false;

	const unsigned int index0 = r0 * cols + c0;

	IsoObject * obj = mObjectsMap[index0];

	// nothing to move
	if (!obj)
		return false;

	const unsigned int index1 = r1 * cols + c1;

	// cell is full
	if (mObjectsMap[index1])
		return false;

	// re-position object
	//if (updatePosition)
	//PositionObject(obj, r1, c1);

	// remove object
	ClearObjectFromMap(obj);
	RemoveObjectFromRenderList(obj);

	// need to update object position before adding it again
	obj->SetRow(r1);
	obj->SetCol(c1);

	// add object back
	InsertObjectInMap(obj);

	UpdateRenderList();

	return true;
}

void IsoLayer::SetObjectVisible(IsoObject * obj, bool visible)
{
	obj->SetVisible(visible);

	if (visible)
	{
		// object is not already in
		if (std::find(mRenderList.begin(), mRenderList.end(), obj) == mRenderList.end())
			UpdateRenderList();
	}
	else
		RemoveObjectFromRenderList(obj);
}

/// Destroy all the objects
void IsoLayer::ClearObjects()
{
	// reset Layer link
	for (auto * obj : mObjectsList)
		obj->SetLayer(nullptr);

	// clear lists
	mObjectsList.clear();
	//mRenderList.clear();

	const unsigned int mapsSize = 40 * 40;
	mObjectsMap.assign(mapsSize, nullptr);
}

/// Renders all the objects in the layer.
void IsoLayer::Render(Matrix4f& transform)
{
	/*for (auto * obj : mRenderList) {
	obj->Render();
	}*/

	for (auto * obj : mObjectsList) {
		obj->Render(transform);
	}
}

void IsoLayer::MoveObjectsPosition(int deltaX, int deltaY)
{
	for (auto * obj : mObjectsList)
	{
		const int x = obj->GetX() + deltaX;
		const int y = obj->GetY() + deltaY;

		obj->SetPosition(x, y);
	}
}

void IsoLayer::RepositionObject(IsoObject * obj){
	PositionObject(obj, obj->GetRow(), obj->GetCol());
}

Vector2f IsoLayer::GetObjectPosition(const IsoObject * obj, int r,  int c) const{
	const Vector2f cellPos = mMap->GetCellPosition((40 -3) - r, (40 - 3) - c);
	
	const int cellH = mMap->GetTileHeight();

	const int x0 = cellPos[0] + cellH + 800;
	const int y0 = cellPos[1] + cellH - 510;

	const int imgW0 = obj->GetCols() * cellH;
	const int imgH = obj->GetHeight();

	float pointX, pointY;
	float pointXTrans, pointYTrans;

	pointX = (r) * (float)(TILE_WIDTH) * 0.5f;
	pointY = (c ) * (float)(TILE_WIDTH) * 0.5f;
	pointXTrans = (pointX - pointY);
	pointYTrans = (pointX + pointY) * 0.5f;

	return Vector2f(cellPos[0] - TILE_WIDTH, cellPos[1]);
}

// ==================== PRIVATE METHODS ====================

void IsoLayer::PositionObject(IsoObject * obj, unsigned int r, unsigned int c){

	const Vector2f pos = GetObjectPosition(obj, r, c);
	obj->SetPosition(pos[0], pos[1]);
}

/**
* @brief Removes an object from a cell. Object is not deleted.
* @param index Position index in the layer. Identifies the cell
*/
void IsoLayer::ClearObject(unsigned int index)
{
	IsoObject * obj = mObjectsMap[index];

	ClearObject(obj);
}

void IsoLayer::RemoveObjectFromList(IsoObject * obj)
{
	auto it = std::find(mObjectsList.begin(), mObjectsList.end(), obj);

	if (it != mObjectsList.end())
		mObjectsList.erase(it);
}

void IsoLayer::RemoveObjectFromRenderList(IsoObject * obj)
{
	auto it = std::find(mRenderList.begin(), mRenderList.end(), obj);

	if (it != mRenderList.end())
		mRenderList.erase(it);
}

void IsoLayer::UpdateRenderList()
{
	//mRenderList.clear();

	const int cols = 40;
	for (int r = mRenderingR0; r < mRenderingR1; ++r)
	{
		const int ind0 = r * cols;

		for (int c = mRenderingC0; c < mRenderingC1; ++c)
		{
			const int ind = ind0 + c;

			IsoObject * obj = mObjectsMap[ind];

			if (obj != nullptr && obj->GetRow() == r && obj->GetCol() == c) {
				mRenderList.push_back(obj);

			}
		}
	}

}

void IsoLayer::ClearObjectFromMap(IsoObject * obj)
{
	const int mapCols = 40;

	const int r0 = obj->GetRow();
	const int c0 = obj->GetCol();

	const int r1 = 1 + r0 - obj->GetRows();
	const int c1 = 1 + c0 - obj->GetCols();

	// store object
	for (int r = r1; r <= r0; ++r)
	{
		const unsigned int indBase = r * mapCols;

		for (int c = c1; c <= c0; ++c)
		{
			const unsigned int ind = indBase + c;
			mObjectsMap[ind] = nullptr;
		}
	}
}

void IsoLayer::InsertObjectInMap(IsoObject * obj)
{
	const int mapCols = 40;

	const int r0 = obj->GetRow();
	const int c0 = obj->GetCol();

	const int r1 = 1 + r0 - obj->GetRows();
	const int c1 = 1 + c0 - obj->GetCols();

	// store object
	for (int r = r1; r <= r0; ++r)
	{
		const unsigned int indBase = r * mapCols;

		for (int c = c1; c <= c0; ++c)
		{
			const unsigned int ind = indBase + c;
			mObjectsMap[ind] = obj;
		}
	}
}