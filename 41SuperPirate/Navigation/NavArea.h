#pragma once

#include "../engine/DebugRenderer.h"
#include "../engine/BoundingBox.h"

class NavArea{

	friend class NavigationMesh;
	friend class DynamicNavigationMesh;
	 
public:

	NavArea();
	NavArea(const BoundingBox& boudingBox);
	NavArea(NavArea const& rhs);
	NavArea(NavArea&& rhs);
	NavArea& operator=(const NavArea& rhs);
	NavArea& operator=(NavArea&& rhs);
	virtual ~NavArea();

	void OnRenderDebug() const;

	unsigned getAreaID() const;
	void setAreaID(unsigned int newID) const;
	void setBoundingBox(const BoundingBox& bnds) const;
	const BoundingBox& getBoundingBox() const;
	const bool isEnabled() const;

private:

	mutable BoundingBox m_boundingBox;
	mutable unsigned char m_areaID;
	bool m_isEnabled;
};