#pragma once

#include "../engine/DebugRenderer.h"
#include "../engine/BoundingBox.h"

class NavArea{

	friend class NavigationMesh;
	friend class DynamicNavigationMesh;
	 
public:

	NavArea();
	virtual ~NavArea();

	void OnRenderDebug();

	unsigned getAreaID() const;
	void setAreaID(unsigned newID);
	void setBoundingBox(const BoundingBox& bnds);
	const BoundingBox& getBoundingBox() const;
	const bool isEnabled() const;

private:

	BoundingBox m_boundingBox;
	unsigned char m_areaID;
	bool m_isEnabled;
};