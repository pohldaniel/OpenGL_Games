#pragma once

#include "../engine/DebugRenderer.h"
#include "../engine/BoundingBox.h"

class NavPolygon{

	friend class NavigationMesh;
	friend class DynamicNavigationMesh;
	 
public:

	NavPolygon();
	NavPolygon(NavPolygon const& rhs);
	NavPolygon(NavPolygon&& rhs);
	NavPolygon& operator=(const NavPolygon& rhs);
	NavPolygon& operator=(NavPolygon&& rhs);
	virtual ~NavPolygon();

	void OnRenderDebug() const;

	unsigned getAreaID() const;
	void setAreaID(unsigned int newID) const;
	const bool isEnabled() const;
	const BoundingBox& getBoundingBox() const;
	void createBoundingBox();

	void setNumVerts(int numVerts);
	void setMinY(float minY);
	void setMaxY(float maxY);
	void setVerts(float* verts);

private:
	BoundingBox m_boundingBox;
	mutable unsigned char m_areaID;
	bool m_isEnabled;
	float *m_verts;
	float m_minY;
	float m_maxY;
	int m_numVerts;
};