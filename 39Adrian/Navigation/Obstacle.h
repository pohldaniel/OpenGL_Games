#pragma once

#include <array>
#include "../engine/scene/OctreeNode.h"


class DynamicNavigationMesh;

class Obstacle {

	friend class DynamicNavigationMesh;

public:

	Obstacle(OctreeNode* node);
	virtual ~Obstacle();

	void OnSetEnabled();
	void OnRenderDebug();
	void OnTileAdded(const std::array<int, 2>& tile);

	void setOwnerMesh(DynamicNavigationMesh* ownerMesh);
	void setNode(OctreeNode* node);

	void setIsEnabled(bool isEnabled);
	
	void setHeight(float newHeight);
	void setRadius(float newRadius);

	OctreeNode* getNode();
	float getHeight() const;
	float getRadius() const;
	unsigned getObstacleID() const;
	
private:

	OctreeNode* m_node;
	DynamicNavigationMesh* m_ownerMesh;
	float m_radius;
	float m_height;
	unsigned m_obstacleId;
	bool m_isEnabled;
};