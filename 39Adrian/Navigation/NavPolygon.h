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

private:

	mutable unsigned char m_areaID;
	bool m_isEnabled;
	float *m_verts;
	float m_minY;
	float m_maxY;
	int m_numVerts;
};