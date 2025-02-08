#pragma once

#include "NavigationMesh.h"

class OffMeshConnection;
class Obstacle;

class DynamicNavigationMesh : public NavigationMesh {

public:

	/// Used by Obstacle class to add itself to the tile cache, if 'silent' an event will not be raised.
	void AddObstacle(Obstacle* obstacle, bool silent = false);
	/// Used by Obstacle class to update itself.
	void ObstacleChanged(Obstacle* obstacle);
	/// Used by Obstacle class to remove itself from the tile cache, if 'silent' an event will not be raised.
	void RemoveObstacle(Obstacle*, bool silent = false);
};