#pragma once

#include <string>
#include <vector>

#include "engine/Vector.h"

class IsoMap;
class IsoObject;
class IsoLayer{

public:
	IsoLayer(const IsoMap * map);

	void UpdateSize();

	bool ContainsObject(unsigned int r, unsigned int c) const;
	bool AddObject(IsoObject * obj, unsigned int r, unsigned int c);
	void ClearObject(IsoObject * obj);
	void ClearObject(unsigned int r, unsigned int c);
	bool MoveObject(unsigned int r0, unsigned int c0,
		unsigned int r1, unsigned int c1,
		bool updatePosition = true);

	void SetObjectVisible(IsoObject * obj, bool visible);

	void ClearObjects();

	bool IsVisible() const;
	void SetVisible(bool val);
	void SetRenderingCells(unsigned int r0, unsigned int c0, unsigned int r1, unsigned int c1);
	void Render(Matrix4f& transform);

	void MoveObjectsPosition(int deltaX, int deltaY);

	void RepositionObject(IsoObject * obj);
	Vector2f GetObjectPosition(const IsoObject * obj, int r, int c) const;

	void PositionObject(IsoObject * obj, unsigned int r, unsigned int c);
	void ClearObject(unsigned int index);

	void RemoveObjectFromList(IsoObject * obj);

	void RemoveObjectFromRenderList(IsoObject * obj);

	void UpdateRenderList();

	void ClearObjectFromMap(IsoObject * obj);
	void InsertObjectInMap(IsoObject * obj);


	std::vector<IsoObject *> mObjectsList;
	std::vector<IsoObject *> mRenderList;
	std::vector<IsoObject *> mObjectsMap;

	unsigned int mRenderingR0 = 0;
	unsigned int mRenderingR1 = 0;
	unsigned int mRenderingC0 = 0;
	unsigned int mRenderingC1 = 0;

	bool mVisible = true;
	const IsoMap * mMap = nullptr;
};

inline bool IsoLayer::IsVisible() const { return mVisible; }

inline void IsoLayer::SetVisible(bool val) { mVisible = val; }

inline void IsoLayer::SetRenderingCells(unsigned int r0, unsigned int c0, unsigned int r1, unsigned int c1) {
	// nothing has changed
	if (mRenderingR0 == r0 && mRenderingR1 == r1 && mRenderingC0 == c0 && mRenderingC1 == c1)
		return;

	mRenderingR0 = r0;
	mRenderingR1 = r1;
	mRenderingC0 = c0;
	mRenderingC1 = c1;

	UpdateRenderList();
}